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

is_win = platform.system() == 'Windows'
is_mac = platform.system() == 'Darwin'
is_linux = platform.system() == 'Linux'
assert(is_win or is_mac or is_linux)


def detect_compiler_version():
    compiler = {
        'clang': None,
        'gcc': None,
        'msvc': None
    }
    version_re = re.compile(r'[^\n\d]*(\d+)\.(\d+)\.(\d+).*')
    if is_mac or is_linux:
        try:
            output = subprocess.check_output(['clang++', '--version'])
            m = version_re.match(output.decode())
            compiler['clang'] = {'version': tuple(int(v) for v in m.groups())}
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
        vswhere_path = os.path.join(
            os.path.dirname(__file__), 'win', 'vswhere.exe')
        output = subprocess.check_output([vswhere_path, '-format', 'json'])
        import json
        vss = json.loads(output)
        if len(vss):
            compiler['msvc'] = []
            for vs in vss:
                compiler['msvc'].append(vs)
                vcvarsall = os.path.join(
                    vs['installationPath'], "VC\\Auxiliary\\Build\\vcvarsall.bat")
                params = [vcvarsall, 'x86', '&&', 'cl.exe']
                output = subprocess.check_output(
                    params, stderr=subprocess.STDOUT, shell=True)
                m = re.search(
                    r'Compiler Version[^\n\d]+(\d+)\.(\d+)\.(\d+)', output)
                # for x86 could be parsed
                assert(m)
                assert('version' not in compiler['msvc'][-1])
                compiler[
                    'msvc'][-1]['version'] = tuple(int(v) for v in m.groups())
                assert('vcvarsall' not in compiler['msvc'][-1])
                compiler['msvc'][-1]['vcvarsall'] = vcvarsall
    print('Detected compiler', compiler)
    return compiler


local_compiler = detect_compiler_version()


class Value:

    def __init__(self, arg, value, short):
        self.arg = arg
        self.value = value
        self.short = short

    def __call__(self):
        return self.value

    def __eq__(self, other):
        assert(self.arg == other.arg)
        return self.value == other.value

    def __repr__(self):
        return self.value


class BooleanArg:

    def __init__(self, name, short):
        self.name = name
        self.short = short
        self.true = Value(self, 'true', 't')
        self.false = Value(self, 'false', 'f')
        self.values = (self.true, self.false)

    def __repr__(self):
        return self.name


class StringArg:

    def __init__(self, name, short, **kwargs):
        self.name = name
        self.short = short
        self.values = []
        assert(len(kwargs))
        assert('values' not in kwargs)
        for key in kwargs:
            self.values.append(Value(self, *kwargs[key]))
            self.__dict__[key] = self.values[-1]

    def __repr__(self):
        return self.name


class GN:

    def __init__(self, **kwargs):
        assert('args' not in kwargs)
        self.args = dict()
        self.filters = []
        self.add_args(**kwargs)

    def add_args(self, **kwargs):
        assert('args' not in kwargs)
        for k in kwargs:
            assert(k not in self.args)
        self.args.update(kwargs)
        self.__dict__.update(self.args)

    def __repr__(self):
        return repr({k: repr(self.args[k].values) for k in self.args})

    def add_permanent_filter(self, func):
        self.filters.append(func)

    def add_permanent_filter_not(self, func):
        self.filters.append(lambda x: not func(x))

    def register_to_globals(self):
        for key in self.args:
            assert(key not in globals())
            globals()[key] = self.args[key]

    def descardian(self):
        variants = [{}]
        for key in self.args:
            extended_variants = []
            for variant in variants:
                for value in self.args[key].values:
                    extended_variants.append(variant.copy())
                    extended_variants[-1][self.args[key]] = value
            variants = extended_variants

        class Descardian:

            def __init__(self, variants):
                self.variants = variants

            def filter(self, func):
                return Descardian(filter(func, self.variants))

            def filter_not(self, func):
                return self.filter(lambda x: not func(x))

            def __repr__(self):
                return repr(list(self.variants))

            def __len__(self):
                return len(list(self.variants))

            def __iter__(self):
                return self.variants.__iter__()

        return Descardian(filter(lambda v: all(f(v) for f in self.filters), variants))


gn = GN(
    is_debug=BooleanArg('is_debug', 'debug'),
    is_run_tests=BooleanArg('is_run_tests', 'tests'),
    is_run_performance_tests=BooleanArg('is_run_performance_tests', 'perf'),
    is_asan=BooleanArg('is_asan', 'asan'),
    is_generate_test_coverage=BooleanArg('is_generate_test_coverage', 'cov'),
    compiler_type=StringArg('compiler_type', 'c', clang=(
        '"clang"', 'clang'), gcc=('"gcc"', 'gcc'), msvc=('"msvc"', 'msvc')),
    std_version=StringArg('std_version', 'std', cpp11=('"c++11"', '11'), cpp14=(
        '"c++14"', '14'), cpp17=('"c++17"', '17'), cpplatest=('"c++latest"', 'latest')),
    define_macros=StringArg('define_macros', 'defs',
                            no_macro=('[]', 'e'),
                            assert_on=('["JASL_ASSERT_ON"]', 'assert'),
                            terminate_on=(
                                '["JASL_TERMINATE_ON_EXCEPTION_ON"]', 'exc'),
                            assert_and_terminate_on=(
                                '["JASL_ASSERT_ON", "JASL_TERMINATE_ON_EXCEPTION_ON"]', 'assert-exc')
                            )
)

gn.add_permanent_filter_not(
    lambda x: x[gn.compiler_type] != gn.compiler_type.clang and x[gn.is_asan] == gn.is_asan.true)
gn.add_permanent_filter_not(
    lambda x: x[gn.compiler_type] != gn.compiler_type.clang and x[gn.is_generate_test_coverage] == gn.is_generate_test_coverage.true)
gn.add_permanent_filter_not(lambda x: x[gn.is_generate_test_coverage] ==
                            gn.is_generate_test_coverage.true and x[gn.is_run_tests] == gn.is_run_tests.false)
gn.add_permanent_filter_not(lambda x: x[gn.define_macros] in [
                            gn.define_macros.terminate_on, gn.define_macros.assert_and_terminate_on] and x[gn.is_run_tests] == gn.is_run_tests.true)
gn.add_permanent_filter_not(
    lambda x: x[gn.compiler_type] == gn.compiler_type.msvc and x[gn.std_version] == gn.std_version.cpp11)
gn.add_permanent_filter_not(
    lambda x: x[gn.compiler_type] != gn.compiler_type.msvc and x[gn.std_version] == gn.std_version.cpplatest)

toolchain = [gn.compiler_type.clang] if is_mac else [gn.compiler_type.gcc,
                                                     gn.compiler_type.clang] if is_linux else [gn.compiler_type.msvc] if is_win else None

gn.add_permanent_filter(lambda x: x[gn.compiler_type] in toolchain)


def as_dir(variant):
    return '_'.join(sorted(arg.short + variant[arg].short.title() for arg in variant))


def as_args(variant):
    return ' '.join(sorted(arg.name + '=' + variant[arg].value for arg in variant))


def current_platform_default_compiler_type():
    return gn.compiler_type.clang if is_mac else gn.compiler_type.gcc if is_linux else gn.compiler_type.msvc if is_win else None


########
########
########
if __name__ == '__main__':
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
    args = parser.parse_args()

    if args.travis_ci or args.appveyor:
        args.gen = True
        args.ninja = True

    # remark: vswhere.exe somehow not working on appveyor
    if args.msvc_vcvarsall_path:
        assert(is_win)
        assert(not args.compiler_type or args.compiler_type == 'msvc')
        assert(os.path.exists(args.msvc_vcvarsall_path))
        gn.add_args(visual_studio_path=StringArg(
                'visual_studio_path', 'vs', param=('"' + args.msvc_vcvarsall_path + '"', 'param')))
    elif is_win:
        # vswhere.exe should have found the compilers
        assert(local_compiler['msvc'])
        vs_vers = {}
        for vs in local_compiler['msvc']:
            instanceId = 'id' + vs['instanceId']
            vs_vers[instanceId] = ('"' + vs['vcvarsall'] + '"', instanceId)
        gn.add_args(visual_studio_path=StringArg(
            'visual_studio_path', 'vs', **vs_vers))

    ###
    if args.clean:
        for item in filter(lambda x: x not in ['.clang-format'], os.listdir('out')):
            shutil.rmtree(os.path.join('out', item), ignore_errors=True)

    variants = gn.descardian()

    # test generation with existing toolchains
    if is_mac:
        variants = variants.filter(
            lambda x: x[gn.compiler_type] == gn.compiler_type.clang)
    elif is_linux:
        variants = variants.filter(lambda x: x[gn.compiler_type] in [
                                   gn.compiler_type.clang, gn.compiler_type.gcc])
    elif is_win:
        variants = variants.filter(
            lambda x: x[gn.compiler_type] == gn.compiler_type.msvc)
    else:
        assert(False)

    if args.compiler_type:
        if args.compiler_type == 'clang':
            filter_this = gn.compiler_type.clang
        elif args.compiler_type == 'gcc':
            filter_this = gn.compiler_type.gcc
        elif args.compiler_type == 'msvc':
            filter_this = gn.compiler_type.msvc
        variants = variants.filter(
            lambda x: x[gn.compiler_type] == filter_this)

    ###
    if args.gen:
        gn_exec = 'gn' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants:
            out_dir = os.path.join('out', as_dir(v))
            command = [gn_exec, 'gen', out_dir, '--args=' + as_args(v)]
            print(' '.join(command))
            sys.stdout.flush()
            return_code = subprocess.call(command)
            if return_code != 0:
                print('Error: ' + str(return_code))
                fail_count += 1
                raise Exception()
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    # filter more, build less
    variants = variants.filter(
        lambda x: x[gn.is_run_performance_tests] == gn.is_run_performance_tests.false)

    if args.travis_ci:
        variants = variants.filter(lambda x: x[gn.is_asan] == gn.is_asan.false)
        variants = variants.filter(
            lambda x: x[gn.is_generate_test_coverage] == gn.is_generate_test_coverage.false)
    elif not args.compiler_type:  # local testing
        variants = variants.filter(
            lambda x: x[gn.compiler_type] == current_platform_default_compiler_type())

    if local_compiler['clang']:
        if is_mac and local_compiler['clang']['version'][0] < 9:
            variants = variants.filter_not(
                lambda x: x[gn.compiler_type] == gn.compiler_type.clang and x[gn.std_version] == gn.std_version.cpp17)
    if local_compiler['gcc']:
        if local_compiler['gcc']['version'][0] < 5:
            variants = variants.filter_not(
                lambda x: x[gn.compiler_type] == gn.compiler_type.gcc and x[gn.std_version] in [gn.std_version.cpp14, gn.std_version.cpp17])
    if local_compiler['msvc']:
        pass

    assert(len(variants) > 0)

    ###
    if args.ninja:
        ninja_exec = 'ninja' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants:
            out_dir = os.path.join('out', as_dir(v))
            print('# ///////////////////')
            print('# ' + as_args(v))
            command = [ninja_exec, '-C', out_dir]
            sys.stdout.flush()
            return_code = subprocess.call(command)
            if return_code != 0:
                fail_count += 1
                print('Error: ' + str(return_code))
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    sys.exit(0)
