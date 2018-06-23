#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import io
import sys
import subprocess
import argparse
import re

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
    lines = ''.join(open(args.touch_on_success, 'r').readlines())

    if return_code == 0:
        print(lines)
        raise Exception('Compiled, but error was expected.')

    if re.search(error_regex, lines, re.MULTILINE) is None:
        print(lines)
        raise Exception('Error output should contains:', error_regex)

    open(args.touch_on_success, 'w').close()
    sys.exit(0)
