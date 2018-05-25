#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import re
import argparse


def version_consistency(args):
    with open(args.jasl_common, 'r') as f:
        jasl_common_lines = '\n'.join(f.readlines())

    jasl_common_version = {
        'major': int(re.search(r'#define\s+JASL_VERSION_MAJOR\s+([0-9]+)', jasl_common_lines).group(1)),
        'minor': int(re.search(r'#define\s+JASL_VERSION_MINOR\s+([0-9]+)', jasl_common_lines).group(1)),
        'patch': int(re.search(r'#define\s+JASL_VERSION_PATCH\s+([0-9]+)', jasl_common_lines).group(1)),
        'date': re.search(r'#define\s+JASL_VERSION_DATE\s+"([^"]+)"', jasl_common_lines).group(1)
    }

    with open(args.changelog, 'r') as f:
        changelog_lines = '\n'.join(f.readlines())

    # example:## [0.1.0] - 2018-04-12
    match = re.search(
        r'## \[([0-9]+)\.([0-9]+)\.([0-9]+)\] - (\S+)', changelog_lines)

    changelog_version = {
        'major': int(match.group(1)),
        'minor': int(match.group(2)),
        'patch': int(match.group(3)),
        'date': match.group(4)
    }

    assert(len(jasl_common_version) == len(changelog_version))
    for k in jasl_common_version.keys():
        if jasl_common_version[k] != changelog_version[k]:
            raise Exception("Version mismatch!", k,
                            jasl_common_version, changelog_version)

    # touch
    open(args.output, 'w').close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    vc_parser = subparsers.add_parser("version-consistency")
    vc_parser.add_argument("--changelog", required=True)
    vc_parser.add_argument("--jasl_common", required=True)
    vc_parser.add_argument("--output", required=True)
    vc_parser.set_defaults(func=version_consistency)

    args = parser.parse_args()
    args.func(args)

    sys.exit(0)
