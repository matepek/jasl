#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# JASL: For more information see https://github.com/matepek/jasl
#
# Copyright (c) 2018 Mate Pek
#
# This code is licensed under the MIT License (MIT).

import os
import sys
import subprocess
import argparse
import platform
import shutil
import re
import tempfile
import json

is_win = platform.system() == 'Windows'
is_mac = platform.system() == 'Darwin'
is_linux = platform.system() == 'Linux'
assert(is_win or is_mac or is_linux)


def detect_compiler_version(vcvarsall=None):
    compiler = {
        'clang': None,
        'gcc': None,
        'msvc': []
    }
    version_re = re.compile(r'[^\n\d]*(\d+)\.(\d+)\.(\d+).*')
    if is_mac or is_linux:
        try:
            output = subprocess.check_output(['clang++', '--version'])
            m = version_re.match(output.decode())
            compiler['clang'] = {'version': tuple(int(v) for v in m.groups())}

            def compiler_info():
                f = tempfile.NamedTemporaryFile(delete=False)
                f.write('''#include <iostream>
                        using namespace std;
                        int main() {
                            cout << '{' << "\\"has_string_view\\":";
                            #if __has_include(<string_view>)
                            cout << "true";
                            #else
                            cout << "false";
                            #endif
                            cout << endl;
                            cout << '}';
                        }
                    ''')
                f.close()
                o = tempfile.NamedTemporaryFile(delete=False)
                o.close()
                res = subprocess.call(
                    ['clang++', '-x', 'c++', f.name, '-o', o.name])
                if res != 0:
                    raise Exception(res)
                output = subprocess.check_output([o.name])
                os.remove(f.name)
                os.remove(o.name)
                clang_prop = json.loads(output)
                compiler['clang'].update(clang_prop)
            compiler_info()
        except subprocess.CalledProcessError as e:
            assert(e.returncode == 2)
    if is_linux:
        try:
            output = subprocess.check_output(['g++', '--version'])
            m = version_re.match(output.decode())
            compiler['gcc'] = {'version': tuple(int(v) for v in m.groups())}
        except subprocess.CalledProcessError as e:
            assert(e.returncode == 2)
    if is_win:
        def get_msvc_version(vcvarsall):
            params = [vcvarsall, 'x86', '&&', 'cl.exe']
            output = subprocess.check_output(
                params, stderr=subprocess.STDOUT, shell=True)
            m = re.search(
                r'Compiler Version[^\n\d]+(\d+)\.(\d+)\.(\d+)', output)
            # for x86 could be parsed
            return {'version': tuple(int(v) for v in m.groups()), 'vcvarsall': vcvarsall}
        if vcvarsall:
            assert(os.path.exists(vcvarsall))
            compiler['msvc'].append(get_msvc_version(vcvarsall))
            compiler['msvc'][-1]['instanceId'] = 'script_arg'
        else:
            vswhere_path = os.path.join(
                os.path.dirname(__file__), 'win', 'vswhere.exe')
            output = subprocess.check_output([vswhere_path, '-format', 'json'])
            vss = json.loads(output)
            if len(vss):
                for vs in vss:
                    assert('instanceId' in vs)
                    vcvarsall = os.path.join(
                        vs['installationPath'], "VC\\Auxiliary\\Build\\vcvarsall.bat")
                    try:
                        vs.update(get_msvc_version(vcvarsall))
                        compiler['msvc'].append(vs)
                    except subprocess.CalledProcessError as e:
                        print("Warning: vswhere found a compiler, but script couldn't locate vcvarsall.bat", vs)
                        assert(e.returncode == 2)
    return compiler


class Value:

    def __init__(self, name, value, short=None):
        self.name = name
        self.value = value
        self.short = short if short else name

    def __call__(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __repr__(self):
        return 'Value(' + repr(self.name) + ', ' + repr(self.value) + ', ' + repr(self.short) + ')'

    def __nonzero__(self):
        asser(False)


class TrueValue(Value):

    def __nonzero__(self):
        return True


class FalseValue(Value):

    def __nonzero__(self):
        return False


class BooleanArg:

    def __init__(self, name, short):
        self.name = name
        self.short = short
        self.true = TrueValue('true', 'true', 't')
        self.false = FalseValue('false', 'false', 'f')
        self.values = (self.true, self.false)

    def __repr__(self):
        return 'BooleanArg(' + repr(self.name) + ', ' + repr(self.short) + ')'


class StringArg:

    def __init__(self, name, short, values=[]):
        self.name = name
        self.short = short
        self.values = []
        for v in values:
            self.add(v)

    def add(self, value):
        assert(value.name not in self.__dict__)
        self.__dict__[value.name] = value
        self.values.append(value)
        return self

    def __repr__(self):
        return 'StringArg(' + repr(self.name) + ', ' + repr(self.short) + ', ' + repr(list(self.values)) + ')'


class GN:

    def __init__(self, args=[]):
        self._args = dict()
        self._filters = []
        for a in args:
            self.add(a)

    def add(self, arg):
        assert(arg.name not in self.__dict__)
        assert(arg.name not in self._args)
        self.__dict__[arg.name] = arg
        self._args[arg.name] = arg
        return arg

    def __repr__(self):
        return 'GN(' + repr(list(self._args[a] for a in self._args)) + ')'

    def filter(self, func):
        self._filters.append(func)

    def filter_not(self, func):
        self._filters.append(lambda x: not func(x))

    def variants(self):
        class Variant:

            def __init__(self, arg_value_dict={}):
                self.arg_value_dict = arg_value_dict

            def copy(self):
                return Variant(self.arg_value_dict.copy())

            def as_dir(self):
                # warning: On windows path name has a limit. We should carefully generate directory names.
                return '_'.join(sorted(arg.short + '' + self.arg_value_dict[arg].short.title() for arg in self.arg_value_dict))

            def as_args(self):
                return ' '.join(sorted(arg.name + '=' + self.arg_value_dict[arg].value for arg in self.arg_value_dict))

            def __getattr__(self, key):
                key_value = {
                    k.name: self.arg_value_dict[k] for k in self.arg_value_dict}
                assert(key in key_value)
                return key_value[key]

            def __repr__(self):
                return repr({k.name: self.arg_value_dict[k].value for k in self.arg_value_dict})

        variants = [Variant()]
        for arg_name in self._args:
            extended_variants = []
            for variant in variants:
                for value in self._args[arg_name].values:
                    extended_variants.append(variant.copy())
                    extended_variants[-1].arg_value_dict[self._args[arg_name]] = value
            variants = extended_variants

        class Variants:

            def __init__(self, variants):
                self.variants = variants

            def filter(self, func):
                self.variants = filter(func, self.variants)

            def filter_not(self, func):
                self.filter(lambda x: not func(x))

            def __repr__(self):
                self.variants = list(self.variants)
                return repr(self.variants)

            def __len__(self):
                self.variants = list(self.variants)
                return len(self.variants)

            def __iter__(self):
                return self.variants.__iter__()

        return Variants(filter(lambda v: all(f(v) for f in self._filters), variants))


########
########
########
if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    assert(os.getcwd().endswith('jasl'))

    parser = argparse.ArgumentParser()
    parser.add_argument('--clean', '-c', action='store_true')
    parser.add_argument('--gen', '--gn', '-g', action='store_true')
    parser.add_argument('--ninja', '-n', action='store_true')
    parser.add_argument('--travis-ci', action='store_true')
    parser.add_argument('--appveyor', action='store_true')
    parser.add_argument('--compiler-type',
                        choices=['msvc', 'clang', 'gcc'])
    parser.add_argument('--msvc-vcvarsall-path')
    script_arg = parser.parse_args()

    if script_arg.appveyor:
        assert(is_win)
    if script_arg.travis_ci:
        assert(is_mac or is_linux)

    gn = GN()
    gn.add(BooleanArg('is_debug', 'd'))
    gn.add(BooleanArg('is_run_tests', 't'))
    gn.add(BooleanArg('is_run_performance_tests', 'p'))
    gn.add(BooleanArg('is_asan', 'asan'))
    gn.add(BooleanArg('is_generate_test_coverage', 'cov'))
    gn.add(BooleanArg('is_std_string_view_supported', 'sv'))
    gn.add(StringArg('compiler_type', '')) \
        .add(Value('clang', '"clang"')) \
        .add(Value('gcc', '"gcc"')) \
        .add(Value('msvc', '"msvc"'))
    gn.add(StringArg('std_version', '')) \
        .add(Value('cpp11', '"c++11"')) \
        .add(Value('cpp14', '"c++14"')) \
        .add(Value('cpp17', '"c++17"')) \
        .add(Value('cpplatest', '"c++latest"'))
    gn.add(StringArg('define_macros', 'm')) \
        .add(Value('macro_off', '[]')) \
        .add(Value('assert_on', '["JASL_ASSERT_ON"]')) \
        .add(Value('terminate_on', '["JASL_TERMINATE_ON_EXCEPTION_ON"]')) \
        .add(Value('assert_on_term_on', '["JASL_ASSERT_ON", "JASL_TERMINATE_ON_EXCEPTION_ON"]'))

    # These filters exlude illegal variations
    gn.filter_not(lambda x: x.compiler_type !=
                  gn.compiler_type.clang and x.is_asan)
    gn.filter_not(lambda x: x.compiler_type not in [
                  gn.compiler_type.clang, gn.compiler_type.gcc] and x.is_generate_test_coverage)
    gn.filter_not(lambda x: x.is_generate_test_coverage and not x.is_run_tests)
    gn.filter_not(lambda x: x.is_generate_test_coverage and not x.is_debug)
    gn.filter_not(
        lambda x: x.is_std_string_view_supported and x.std_version != gn.std_version.cpp17)
    gn.filter_not(lambda x: x.define_macros in [
                  gn.define_macros.terminate_on, gn.define_macros.assert_on_term_on] and x.is_run_tests)
    gn.filter_not(lambda x: x.compiler_type ==
                  gn.compiler_type.msvc and x.std_version == gn.std_version.cpp11)
    gn.filter_not(lambda x: x.compiler_type !=
                  gn.compiler_type.msvc and x.std_version == gn.std_version.cpplatest)
    # existing toolchain
    if is_mac:
        gn.filter(lambda x: x.compiler_type in [gn.compiler_type.clang])
    elif is_linux:
        gn.filter(lambda x: x.compiler_type in [
                  gn.compiler_type.gcc, gn.compiler_type.clang])
    elif is_win:
        gn.filter(lambda x: x.compiler_type in [gn.compiler_type.msvc])

    if script_arg.travis_ci or script_arg.appveyor:
        script_arg.gen = True
        script_arg.ninja = True

    # remark: vswhere.exe somehow not working on appveyor
    local_compiler = detect_compiler_version(
        vcvarsall=script_arg.msvc_vcvarsall_path)
    print('Detected compiler: ' + repr(local_compiler))

    if is_win:
        assert(local_compiler['msvc'])
        vs_arg = gn.add(StringArg('visual_studio_path', 'vs'))
        for vs in local_compiler['msvc']:
            vs_arg.add(Value(vs['instanceId'], '"' + vs['vcvarsall'] + '"'))

    # clean
    if script_arg.clean:
        for item in filter(lambda x: x not in ['.clang-format'], os.listdir('out')):
            shutil.rmtree(os.path.join('out', item), ignore_errors=True)

    variants = gn.variants()

    #
    if script_arg.compiler_type:
        if script_arg.compiler_type == 'clang':
            variants.filter(lambda x: x.compiler_type ==
                            gn.compiler_type.clang)
        elif script_arg.compiler_type == 'gcc':
            variants.filter(lambda x: x.compiler_type == gn.compiler_type.gcc)
        elif script_arg.compiler_type == 'msvc':
            variants.filter(lambda x: x.compiler_type == gn.compiler_type.msvc)
        else:
            assert(False)

    # gen
    if script_arg.gen:
        gn_exec = 'gn' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants:
            out_dir = os.path.join('out', v.as_dir())
            command = [gn_exec, 'gen', out_dir, '--args=' + v.as_args()]
            print(' '.join(command))
            sys.stdout.flush()
            try:
                return_code = subprocess.call(command)
            except KeyboardInterrupt:
                sys.exit(1)
            if return_code != 0:
                print('Error: ' + str(return_code))
                fail_count += 1
                raise Exception()
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    # Filter more, build less. These builds are not so importants
    variants.filter(lambda x: not x.is_run_performance_tests)

    if script_arg.travis_ci:
        variants.filter(lambda x: not x.is_asan)  # TODO why?
        variants.filter(lambda x: not x.is_generate_test_coverage)

    if local_compiler['clang']:
        if is_mac and local_compiler['clang']['version'][0] < 9:
            variants.filter_not(lambda x: x.compiler_type ==
                                gn.compiler_type.clang and x.std_version == gn.std_version.cpp17)
        if not local_compiler['clang']['has_string_view']:
            variants.filter_not(
                lambda x: x.std_version == gn.std_version.cpp17 and x.is_std_string_view_supported)
    if local_compiler['gcc']:
        if local_compiler['gcc']['version'][0] < 5:
            variants.filter_not(lambda x: x.compiler_type == gn.compiler_type.gcc and x.std_version in [
                                gn.std_version.cpp14, gn.std_version.cpp17])
    if local_compiler['msvc']:
        for vc in local_compiler['msvc']:
            if vc['version'][0] > 19 or (vc['version'][0] == 19 and vc['version'][1] >= 10):
                continue
            variants.filter_not(
                lambda x: x.visual_studio_path == getattr(gn.visual_studio_path, vc['instanceId']) and x.is_std_string_view_supported)

    assert(len(variants) > 0)

    # ninja
    if script_arg.ninja:
        ninja_exec = 'ninja' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants:
            out_dir = os.path.join('out', v.as_dir())
            print('# ///////////////////')
            print('# ' + v.as_args())
            command = [ninja_exec, '-C', out_dir]
            sys.stdout.flush()
            try:
                return_code = subprocess.call(command)
            except KeyboardInterrupt:
                sys.exit(1)
            if return_code != 0:
                fail_count += 1
                print('Error: ' + str(return_code))
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    sys.exit(0)
