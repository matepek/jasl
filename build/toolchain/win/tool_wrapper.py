#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    stamp_parser = subparsers.add_parser("stamp")
    stamp_parser.add_argument(
        dest="stamp_files", nargs=argparse.REMAINDER, default=[])
    args = parser.parse_args()

    for path in args.stamp_files:
        open(path, 'w').close()

    sys.exit(0)
