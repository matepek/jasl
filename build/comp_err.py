#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import io
import sys
import subprocess
import argparse
import re

class CompiledButErrorWasExpectedError(Exception):
    pass

class RegexDoesNotMatchError(Exception):
    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--touch-on-success', required=True)
    parser.add_argument(
        dest="parameters",
        nargs=argparse.REMAINDER,
        default=[],
        help="Paramters.")
    args = parser.parse_args()

    output = open(args.touch_on_success, 'w')

    if args.parameters and args.parameters[0] == "--":
        args.parameters = args.parameters[1:]

    prefix = 'jasl-error-regex='
    error_regex = None
    for p in args.parameters:
        if p.startswith(prefix):
            assert(error_regex is None)
            error_regex = p
    assert(error_regex)
    args.parameters.remove(error_regex)
    
    error_regex = error_regex[len(prefix):]

    return_code = subprocess.call(args.parameters, stdout=output, stderr=output)

    output.close()
    compiler_output = ''.join(open(args.touch_on_success, 'r').readlines())

    if return_code == 0:
        print(compiler_output)
        raise CompiledButErrorWasExpectedError(error_regex, compiler_output)

    if re.search(error_regex, compiler_output, re.MULTILINE) is None:
        print(compiler_output)
        raise RegexDoesNotMatchError(error_regex, compiler_output)

    open(args.touch_on_success, 'w').close()
    sys.exit(0)
