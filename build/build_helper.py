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
from gn import *


is_win = platform.system() == 'Windows'
is_mac = platform.system() == 'Darwin'
is_linux = platform.system() == 'Linux'
assert(is_win or is_mac or is_linux)


def ninja_all_in_dir(dir_path, script_arg):
    err_count = 0
    succ_count = 0
    for x in os.listdir(os.path.join(os.getcwd(), dir_path)):
        if x[0] != '.' and os.path.isdir(os.path.join(os.getcwd(), dir_path, x)):
            x_p = os.path.join(dir_path, x)
            print('# ' + str(succ_count) + ' ############')
            if os.path.isfile(os.path.join(dir_path, x, 'args.gn')):
                l = open(os.path.join(dir_path, x, 'args.gn'), 'r').readlines()
                l = [x.strip('\r\n') for x in l]
                print('\n'.join(l))
            result = subprocess.call(['ninja', '-C', x_p])
            if result != 0:
                err_count += 1
                if script_arg.stop_on_error:
                    raise Exception(result)
            else:
                succ_count += 1
    if err_count > 0:
        raise Exception(err_count)


def download_ninja_and_gn(target_dir):
    links = {
        'Linux': {'ninja': 'https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip',
                  'gn': 'https://storage.googleapis.com/chromium-gn/ed8b2bc0617fee4ebd1d1a35e8a5bc168c4ca874'},
        'Darwin': {'ninja': 'https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-mac.zip',
                   'gn': 'https://storage.googleapis.com/chromium-gn/a14b089cbae9c29ecbc781686ada8babac8550af'},
        'Windows': {'ninja': 'https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip',
                    'gn': 'https://storage.googleapis.com/chromium-gn/e93779cab57d5f36100faa4d88524a1e33be7b0f'},
    }
    target_dir = 'out/.bin'
    try:
        os.makedirs(target_dir)
    except OSError:
        pass
    os.environ["PATH"] += os.pathsep + os.path.join(os.getcwd(), target_dir)

    def dl(url, target_path):
        def install_and_import(package):
            import importlib
            try:
                importlib.import_module(package)
            except ImportError as e:
                print(e, 'trying install it')
                import pip
                pip.main(['install', package])
            globals()[package] = importlib.import_module(package)
        try:
            install_and_import('six')
            from six.moves import urllib

            def hook(count_of_blocks, block_size, total_size):
                sys.stdout.write('.')
                sys.stdout.flush()
            print('Downloading: ' + url)
            sys.stdout.flush()
            result = urllib.request.urlretrieve(url, target_path, hook)
            sys.stdout.flush()
        except Exception as e:
            print(e, 'trying other ways')
            sys.stdout.flush()
            if not is_win:
                assert(0 == subprocess.call(['wget', '-O', target_path, url]))
            else:
                assert(False)
        print('done')
        sys.stdout.flush()
    # ninja
    ninja_zip_path = os.path.join(target_dir, 'ninja.zip')
    dl(links[platform.system()]['ninja'], ninja_zip_path)
    import zipfile
    zip_ref = zipfile.ZipFile(ninja_zip_path, 'r')
    zip_ref.extractall(target_dir)
    zip_ref.close()
    os.remove(ninja_zip_path)
    ninja_path = os.path.join(target_dir, 'ninja')
    if is_win:
        ninja_path += '.exe'
    else:
        os.chmod(ninja_path, 744)
    assert(0 == subprocess.call([ninja_path, '--version']))
    # gn
    gn_path = os.path.join(target_dir, 'gn')
    if is_win:
        gn_path += '.exe'
    dl(links[platform.system()]['gn'], gn_path)
    if not is_win:
        os.chmod(gn_path, 744)
    assert(0 == subprocess.call([gn_path, '--version']))


def detect_compilers(vcvarsall=None):
    def compiler_info(exec_file):
        compiler_info_program = '''
        #include <iostream>
        using namespace std;
        int main() {
            cout << '{' << "\\"has_string_view\\":";
            #if defined(__has_include)
            #if __has_include(<string_view>)
            cout << "true";
            #else
            cout << "false";
            #endif
            #else
            cout << "false";
            #endif
            cout << endl;
            cout << '}';
        }
        '''
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(compiler_info_program)
        f.close()
        o = tempfile.NamedTemporaryFile(delete=False)
        o.close()
        res = subprocess.call(
            [exec_file, '-x', 'c++', f.name, '-o', o.name])
        if res != 0:
            raise Exception(res)
        output = subprocess.check_output([o.name])
        os.remove(f.name)
        os.remove(o.name)
        return json.loads(output)
    compilers = []
    version_re = re.compile(r'.*[ \t](\d+)\.(\d+)\.(\d+)[- \t\n].*')
    if is_mac or is_linux:
        used = set()
        usrbin = '/usr/bin'
        usrbincontent = [os.path.join(usrbin, x) for x in os.listdir(usrbin)]
        for c in filter(lambda x: os.path.isfile(x), usrbincontent):
            if not os.path.basename(c).startswith('clang++') and not os.path.basename(c).startswith('g++'):
                continue
            if os.path.basename(c) == 'g++' or (is_linux and os.path.basename(c) == 'clang++'):
                # we dont care about the version-less symlink
                continue
            if os.path.realpath(c) in used:
                continue
            used |= {os.path.realpath(c)}
            if os.path.basename(c).startswith('clang++'):
                compiler_type = 'clang'
            else:
                compiler_type = 'gcc'
            try:
                output = subprocess.check_output([c, '--version'])
            except subprocess.CalledProcessError as e:
                assert(e.returncode == 2)
            m = version_re.match(output.decode())
            compilers.append({'type': compiler_type, 'compiler_exec': c,
                              'version': tuple(int(v) for v in m.groups())})
            compilers[-1].update(compiler_info(c))
            if compiler_type == 'clang':
                # older mac clang hasn't c++17
                if is_mac:
                    compilers[-1]['has_cpp14'] = True
                    compilers[-1]['has_cpp17'] = compilers[-1]['version'][0] >= 9
                    compilers[-1]['has_good_sanitizer'] = True
                if is_linux:
                    compilers[-1]['has_cpp14'] = True
                    compilers[-1]['has_cpp17'] = compilers[-1]['version'][0] > 4 or (
                        compilers[-1]['version'][0] == 4 and compilers[-1]['version'][1] >= 8)
                    compilers[-1]['has_good_sanitizer'] = compilers[-1]['version'][0] >= 6
            elif compiler_type == 'gcc':
                compilers[-1]['has_cpp14'] = compilers[-1]['version'][0] >= 5
                compilers[-1]['has_cpp17'] = compilers[-1]['version'][0] >= 5
                compilers[-1]['has_good_sanitizer'] = False
            else:
                assert(False)
    elif is_win:
        def get_msvc_version(vcvarsall):
            assert(os.path.exists(vcvarsall))
            params = [vcvarsall, 'x86', '&&', 'cl.exe']
            output = subprocess.check_output(
                params, stderr=subprocess.STDOUT, shell=True)
            m = re.search(
                r'Compiler Version[^\n\d]+(\d+)\.(\d+)\.(\d+)', output)
            # for x86 could be parsed
            return {'type': 'msvc', 'version': tuple(int(v) for v in m.groups()), 'vcvarsall': vcvarsall}

        def has_string_view(ver):
            return ver[0] > 19 or (ver[0] == 19 and ver[1] >= 10)
        if vcvarsall:
            assert(os.path.exists(vcvarsall))
            compilers.append(get_msvc_version(vcvarsall))
            compilers[-1]['instanceId'] = 'script_arg'
            compilers[-1]['has_string_view'] = has_string_view(
                compilers[-1]['version'])
            compilers[-1]['has_cpp14'] = True
            # this is not sure, but 19.0 definitely False
            compilers[-1]['has_cpp17'] = compilers[-1]['version'][0] > 19 or (
                compilers[-1]['version'][0] == 19 and compilers[-1]['version'][1] > 0)
            compilers[-1]['has_good_sanitizer'] = False
        else:
            vswhere_path = os.path.join(
                os.path.dirname(__file__), 'win', 'vswhere.exe')
            output = subprocess.check_output([vswhere_path, '-format', 'json'])
            vss = json.loads(output)
            if len(vss):
                for vs in vss:
                    assert('instanceId' in vs)
                    assert('type' not in vs)
                    vcvarsall = os.path.join(
                        vs['installationPath'], "VC\\Auxiliary\\Build\\vcvarsall.bat")
                    vs.update(get_msvc_version(vcvarsall))
                    vs['has_string_view'] = has_string_view(vs['version'])
                    vs['has_cpp14'] = True
                    vs['has_cpp17'] = True
                    vs['has_good_sanitizer'] = False
                    compilers.append(vs)
    for c in compilers:
        assert('has_string_view' in c)
        assert('has_cpp14' in c)
        assert('has_cpp17' in c)
        assert('has_good_sanitizer' in c)
        assert(c['type'] != 'msvc' or 'instanceId' in c)
        assert(c['type'] != 'msvc' or 'vcvarsall' in c)
        assert(c['type'] not in ['clang', 'gcc'] or 'compiler_exec' in c)
    return compilers


# ###############
if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    # a lot of things depends on this assert
    assert(os.getcwd().endswith('jasl'))

    parser = argparse.ArgumentParser()
    parser.add_argument('--matepek', action='store_true')
    parser.add_argument('--quick-ninja', '-q', action='store_true')
    parser.add_argument('--clean', '-c', action='store_true')
    parser.add_argument('--install', '-i', action='store_true')
    parser.add_argument('--stop-on-error', '-e', '-s', action='store_true')
    parser.add_argument('--gen', '--gn', '-g', action='store_true')
    parser.add_argument('--ninja', '-n', action='store_true')
    parser.add_argument('--travis-ci', action='store_true')
    parser.add_argument('--appveyor', action='store_true')
    parser.add_argument('--compiler-type',
                        choices=['msvc', 'clang', 'gcc'])
    parser.add_argument('--compiler-exec-like', nargs='+')
    parser.add_argument('--msvc-vcvarsall-path')
    script_arg = parser.parse_args()

    if script_arg.quick_ninja:
        ninja_all_in_dir('./out', script_arg)
        sys.exit(0)

    gn = GN()
    gn.add(BooleanArg('is_debug', 'd'))
    gn.add(BooleanArg('is_run_tests', 't'))
    gn.add(BooleanArg('is_run_performance_tests', 'p'))
    gn.add(BooleanArg('is_asan', 'asan'))
    gn.add(BooleanArg('is_lsan', 'lsan'))
    gn.add(BooleanArg('is_msan', 'msan'))
    gn.add(BooleanArg('is_usan', 'usan'))

    def is_sanitizer(
        x): return x.is_asan or x.is_lsan or x.is_msan or x.is_usan

    gn.add(BooleanArg('is_generate_test_coverage', 'cov'))
    gn.add(BooleanArg('is_std_string_view_supported', 'sv'))
    gn.add(StringArg('compiler_type', '')) \
        .add(StringValue('clang')) \
        .add(StringValue('gcc')) \
        .add(StringValue('msvc'))
    gn.add(StringArg('std_version', '')) \
        .add(StringValue('cpp11', 'c++11')) \
        .add(StringValue('cpp14', 'c++14')) \
        .add(StringValue('cpp17', 'c++17')) \
        .add(StringValue('cpplatest', 'c++latest'))
    gn.add(StringArg('target_cpu', '')) \
        .add(StringValue('x64')) \
        .add(StringValue('x86'))
    gn.add(BooleanArg('is_defined_JASL_ASSERT_ON', 'as'))
    gn.add(BooleanArg('is_defined_JASL_TERMINATE_ON_EXCEPTION_ON', 'term'))
    gn.add(BooleanArg('is_defined_JASL_FORCE_USE_MURMURHASH_HASH', 'mur'))

    # These filters exclude illegal variations
    gn.filter_out(lambda x: is_sanitizer(
        x) and x.compiler_type != gn.compiler_type.clang)
    gn.filter_out(lambda x: x.is_lsan and not x.is_asan)
    gn.filter_out(lambda x: x.is_asan and x.is_msan)
    gn.filter_out(lambda x: x.is_msan and not x.is_debug)
    gn.filter_out(lambda x: x.compiler_type not in [
                  gn.compiler_type.clang, gn.compiler_type.gcc] and x.is_generate_test_coverage)
    gn.filter_out(lambda x: x.is_generate_test_coverage and not x.is_run_tests)
    gn.filter_out(lambda x: x.is_generate_test_coverage and not x.is_debug)
    gn.filter_out(
        lambda x: x.is_std_string_view_supported and x.std_version != gn.std_version.cpp17)
    # because tests are testing exceptions too
    gn.filter_out(lambda x: x.compiler_type ==
                  gn.compiler_type.msvc and x.std_version == gn.std_version.cpp11)
    gn.filter_out(lambda x: x.compiler_type !=
                  gn.compiler_type.msvc and x.std_version == gn.std_version.cpplatest)
    gn.filter_out(lambda x: x.target_cpu ==
                  gn.target_cpu.x86 and x.compiler_type != gn.compiler_type.msvc)
    gn.filter_out(
        lambda x: x.is_defined_JASL_TERMINATE_ON_EXCEPTION_ON and x.is_run_tests)

    assert(not script_arg.appveyor or is_win)
    assert(not script_arg.travis_ci or is_mac or is_linux)
    if script_arg.travis_ci or script_arg.appveyor:
        script_arg.install = True
        script_arg.gen = True
        script_arg.ninja = True
        script_arg.stop_on_error = True

    # remark: vswhere.exe somehow not working on appveyor
    local_compilers = detect_compilers(
        vcvarsall=script_arg.msvc_vcvarsall_path)
    print('Detected compilers: ' + str(local_compilers))

    gn.filter(lambda x, lc=local_compilers: x.compiler_type in set(
        getattr(gn.compiler_type, c['type']) for c in lc))

    vs_arg = None
    comp_exec = None
    for c in local_compilers:
        if c['type'] == 'msvc':
            if not vs_arg:
                vs_arg = gn.add(StringArg('visual_studio_path', 'vs'))
            vs_arg.add(StringValue(c['instanceId'], c['vcvarsall'], data=c))
            is_current_compiler = lambda x, ii=c['instanceId']: x.visual_studio_path == getattr(
                gn.visual_studio_path, ii)
        elif c['type'] == 'clang':
            if not comp_exec:
                comp_exec = comp_exec = gn.add(StringArg('compiler_exec', ''))
            comp_exec_name = os.path.basename(c['compiler_exec'])
            comp_exec.add(StringValue(comp_exec_name,
                                      c['compiler_exec'], data=c))
            is_current_compiler = lambda x, n=comp_exec_name: x.compiler_exec == getattr(
                gn.compiler_exec, n)
        elif c['type'] == 'gcc':
            if not comp_exec:
                comp_exec = comp_exec = gn.add(StringArg('compiler_exec', ''))
            comp_exec_name = os.path.basename(c['compiler_exec'])
            comp_exec.add(StringValue(comp_exec_name,
                                      c['compiler_exec'], data=c))
            is_current_compiler = lambda x, n=comp_exec_name: x.compiler_exec == getattr(
                gn.compiler_exec, n)
        else:
            assert(False)
        gn.filter_out(lambda x, ct=getattr(
            gn.compiler_type, c['type']), cc=is_current_compiler: cc(x) and x.compiler_type != ct)
        if not c['has_cpp14']:
            gn.filter_out(lambda x, cc=is_current_compiler: cc(
                x) and x.std_version == gn.std_version.cpp14)
        if not c['has_cpp17']:
            gn.filter_out(lambda x, cc=is_current_compiler: cc(
                x) and x.std_version == gn.std_version.cpp17)
        if not c['has_string_view']:
            gn.filter_out(lambda x, cc=is_current_compiler: cc(x)
                          and x.is_std_string_view_supported)
        if not c['has_good_sanitizer']:
            gn.filter_out(lambda x, cc=is_current_compiler: cc(x)
                          and is_sanitizer(x))
    del vs_arg
    del comp_exec
    del local_compilers

    # compiler-exec-like
    if script_arg.compiler_exec_like:
        gn.filter(lambda x, likes=script_arg.compiler_exec_like: any(
            l in x.compiler_exec.value for l in likes))

    # clean
    if script_arg.clean:
        for item in filter(lambda x: x not in ['.clang-format', '.bin'], os.listdir('out')):
            shutil.rmtree(os.path.join('out', item), ignore_errors=True)

    # install
    if script_arg.install:
        download_ninja_and_gn('out/.bin')

    variants = gn.variants()

    #
    if script_arg.compiler_type:
        ct = getattr(gn.compiler_type, script_arg.compiler_type)
        variants.filter(lambda x: x.compiler_type == ct)

    # matepek: for testing
    if script_arg.matepek:
        variants.filter(lambda x: x.std_version ==
                        gn.std_version.cpp17 or x.std_version == gn.std_version.cpp11)
        variants.filter_out(
            lambda x: is_sanitizer(x))
        variants.filter(
            lambda x: x.is_debug and not x.is_generate_test_coverage)
        variants.filter(
            lambda x: not x.is_run_performance_tests and x.is_run_tests)
        variants.filter(lambda x: x.is_defined_JASL_ASSERT_ON)
        variants.filter(
            lambda x: not x.is_defined_JASL_TERMINATE_ON_EXCEPTION_ON)
        variants.filter(
            lambda x: not x.is_defined_JASL_FORCE_USE_MURMURHASH_HASH)
        variants.filter(lambda x: x.target_cpu == gn.target_cpu.x64)

    #
    variants_to_gn = list(variants)
    assert(len(variants_to_gn) > 0)

    variants.filter(lambda x: x.is_run_tests)
    variants.filter_out(lambda x: x.is_run_performance_tests)

    if script_arg.travis_ci:
        if is_mac:
            variants.filter_out(lambda x: is_sanitizer(x))
        if is_linux:
            # clang: error: unsupported argument 'nullability' to option 'fsanitize='
            variants.filter_out(lambda x: is_sanitizer(x) and x.compiler_type == gn.compiler_type.clang and (not x.is_debug or
                                                                                                             x.compiler_exec.data['version'][0] < 6))
            # LeakSanitizer does not work under ptrace (strace, gdb, etc)
            variants.filter_out(lambda x: x.is_lsan or x.is_asan)
        variants.filter(lambda x: not x.is_generate_test_coverage)

    #
    variants_to_ninja = list(variants)
    assert(len(variants_to_ninja) > 0)

    # stat
    print('# GN: ' + str(len(variants_to_gn)) + ' variants to generate.')
    print('# ninja: ' + str(len(variants_to_ninja)) + ' variants to build.')

    # gen
    if script_arg.gen:
        gn_exec = 'gn' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants_to_gn:
            out_dir = os.path.join('out', v.as_dir())
            command = [gn_exec, 'gen', out_dir, '--args=' + v.as_args()]
            print(str(succ_count + fail_count) + ': ' + ' '.join(command))
            sys.stdout.flush()
            try:
                return_code = subprocess.call(command)
            except KeyboardInterrupt:
                sys.exit(1)
            if return_code != 0:
                fail_count += 1
                if script_arg.stop_on_error:
                    raise Exception('stop-on-error', return_code, succ_count)
                print('Error: ' + str(return_code))
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    # ninja
    if script_arg.ninja:
        ninja_exec = 'ninja' + ('.exe' if is_win else '')
        succ_count = 0
        fail_count = 0
        for v in variants_to_ninja:
            out_dir = os.path.join('out', v.as_dir())
            print('# ' + str(succ_count + fail_count) + '. gn args:')
            args = v.as_dict()
            for arg in sorted(args):
                print('#   ' + arg + ' = ' + args[arg])
            command = [ninja_exec, '-C', out_dir]
            sys.stdout.flush()
            try:
                return_code = subprocess.call(command)
            except KeyboardInterrupt:
                sys.exit(1)
            if return_code != 0:
                fail_count += 1
                if script_arg.stop_on_error:
                    raise Exception('stop-on-error', return_code, succ_count)
                print('Error: ' + str(return_code))
            else:
                succ_count += 1
        print('ok(' + str(succ_count) + '), failed(' + str(fail_count) + ')')
        if fail_count:
            raise Exception()

    sys.exit(0)
