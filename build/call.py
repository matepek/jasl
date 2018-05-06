#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import io
import sys
import subprocess
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Run the given command or program.')
    parser.add_argument('-e', '--environment-variable',
                        metavar="<name>=<value>",
                        required=False, default=[],
                        action='append')
    parser.add_argument('-c', '--call', required=True, help="Run this.")
    parser.add_argument('-o', '--output', required=False,
                        help="If it is set, the output will be redirected to the given file.")
    parser.add_argument(
        dest="parameters",
        nargs=argparse.REMAINDER,
        default=[],
        help="Paramters.")
    args = parser.parse_args()

    for p in args.environment_variable:
        kv_pair = p.split("=")
        os.environ[kv_pair[0]] = kv_pair[1]

    output = sys.stdout
    if args.output:
        output = io.open(args.output, "w")

    if args.parameters and args.parameters[0] == "--":
        args.parameters = args.parameters[1:]

    return_code = subprocess.call([args.call] + args.parameters, stdout=output)

    if args.output:
        output.close()

    if return_code != 0:
        print('Error code: ' + str(return_code))
    sys.exit(return_code)
