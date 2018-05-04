#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# JASL: For more information see https://github.com/matepek/jasl
#
# Copyright (c) 2018 Mate Pek
#
# This code is licensed under the MIT License (MIT).

import os
import io
import sys
import subprocess
import argparse
import platform
import shutil

is_win = platform.system() == 'Windows'
is_mac = platform.system() == 'Darwin'
is_linux = platform.system() == 'Linux'
assert(is_win or is_mac or is_linux)


class GNArgs:
  def __init__(self):
    self.args = {}
    self.filters = []

  def add_arg(self, name, short, values):
    assert(name not in self.args)
    for key in self.args:
      if short == self.args[key]['short']:
        raise Exception(name, key, short)
    self.args[name] = {'name': name, 'short': short, 'values': values}

  def add_platform_arg(self, name, short, mac, linux, win):
    if is_mac:
      values = mac
    elif is_linux:
      values = linux
    elif is_win:
      values = win
    self.add_arg(name, short, values)

  def descardian(self):
    variants = [{}]
    for key in self.args:
      extended_variants = []
      for variant in variants:
        for value in self.args[key]['values']:
          extended_variants.append(variant.copy())
          extended_variants[-1][key] = value
      variants = extended_variants
    return variants

  def filtered_descardian(self):
    return filter(lambda v: all(f(v) for f in self.filters), self.descardian())

  def add_filter(self, func):
    self.filters.append(func)

  def add_filter_not(self, func):
    self.add_filter(lambda x: not func(x))

  def get(self):
    out = []
    variants = self.filtered_descardian()
    for variant in variants:
      args = []
      short = []
      for name in sorted(variant):
        args.append(name + '=' + variant[name][0])
        short.append(self.args[name]['short'] + variant[name][1].title())
      out.append((' '.join(sorted(args)), '_'.join(sorted(short)), variant))
    return out


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('--clean', '-c', action='store_true')
  parser.add_argument('--gen', '--gn', '-g', action='store_true')
  parser.add_argument('--ninja', '-n', action='store_true')
  parser.add_argument('--travis-ci', action='store_true')
  parser.add_argument('--filter-compiler_type', choices=['msvc', 'clang', 'gcc'])
  args = parser.parse_args()

  if args.travis_ci:
    args.gen = True
    args.ninja = True

  assert(os.getcwd().endswith('jasl'))

  gn = GNArgs()

  true_v = ('true', 't')
  false_v = ('false', 'f')
  booleans = (true_v, false_v)

  gn.add_arg('is_debug', 'debug', booleans)
  gn.add_arg('is_run_tests', 'tests', booleans)
  gn.add_arg('is_run_performance_tests', 'perf', booleans)
  gn.add_arg('is_asan', 'asan', booleans)
  gn.add_arg('is_generate_test_coverage', 'cov', booleans)

  clang_v = ('"clang"', 'clang')
  gcc_v = ('"gcc"', 'gcc')
  msvc_v = ('"msvc"', 'msvc')

  gn.add_platform_arg('compiler_type', 'c',
                      mac={clang_v},
                      linux={gcc_v, clang_v},
                      win={msvc_v})

  c17_v = ('"c++17"', '17')
  c14_v = ('"c++14"', '14')
  c11_v = ('"c++11"', '11')
  clatest_v = ('"c++latest"', 'latest')

  gn.add_platform_arg('std_version', 'std',
                      mac={c11_v, c14_v, c17_v},
                      linux={c11_v, c14_v, c17_v},
                      win={c14_v, c17_v, clatest_v})

  no_macro = ('[]', 'e')
  assert_on = ('["JASL_ASSERT_ON"]', 'assert')
  abort_on = ('["JASL_ABORT_ON_EXCEPTION_ON"]', 'exc')
  assert_and_abort_on = (
      '["JASL_ASSERT_ON", "JASL_ABORT_ON_EXCEPTION_ON"]', 'assert-exc')

  gn.add_arg('define_macros', 'defs', {
      no_macro,
      assert_on,
      abort_on,
      assert_and_abort_on})

  gn.add_filter_not(lambda x: x['is_asan'] ==
                    true_v and x['compiler_type'] != clang_v)
  gn.add_filter_not(lambda x: x['is_generate_test_coverage']
                    == true_v and x['compiler_type'] != clang_v)
  gn.add_filter_not(lambda x: x['is_generate_test_coverage']
                    == true_v and x['is_run_tests'] == false_v)
  gn.add_filter_not(lambda x: x['define_macros'] in (
      abort_on, assert_and_abort_on) and x['is_run_tests'] == true_v)

  if args.filter_compiler_type:
    if args.filter_compiler_type == 'clang':
      filter_this = clang_v
    elif args.filter_compiler_type == 'gcc':
      filter_this = gcc_v
    elif args.filter_compiler_type =='msvc':
      filter_this = msvc_v
    gn.add_filter(lambda x: x['compiler_type'] == filter_this)

  variants = gn.get()

  ###
  if args.clean:
    shutil.rmtree('out', ignore_errors=True)
    os.mkdir('out')

  ###
  if args.gen:
    gn_exec = 'gn' + ('.exe' if is_win else '')
    succ_count = 0
    fail_count = 0
    for v in variants:
      out_dir = os.path.join('out', v[1])
      command = [gn_exec, 'gen', out_dir, '--args=' + v[0]]
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
  gn.add_filter(lambda x: x['is_run_performance_tests'] == false_v)
  if args.travis_ci:
    gn.add_filter(lambda x: x['is_asan'] == false_v)
    gn.add_filter(lambda x: x['is_generate_test_coverage'] == false_v)
  variants = gn.get()

  ###
  if args.ninja:
    ninja_exec = 'ninja' + ('.exe' if is_win else '')
    succ_count = 0
    fail_count = 0
    for v in variants:
      out_dir = os.path.join('out', v[1])
      print('# ____________________________')
      print('# '+ v[0])
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
