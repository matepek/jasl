#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import re
import argparse


def version_consistency(args):
    def compare(v1, v2):
        version_keys = ('major', 'minor', 'patch', 'label', 'date')
        assert(len(v1) == len(version_keys))
        assert(len(v2) == len(version_keys))
        for k in version_keys:
            assert(v1[k] is not None)
            assert(v2[k] is not None)
            if v1[k] != v2[k]:
                raise Exception("Version mismatch!", k, v1, v2)

    with open(args.jasl_common, 'r') as f:
        jasl_common_lines = '\n'.join(f.readlines())

    jasl_common_version = {
        'major': int(re.search(
            r'#define\s+JASL_VERSION_MAJOR\s+([0-9]+)', jasl_common_lines).group(1)),
        'minor': int(re.search(
            r'#define\s+JASL_VERSION_MINOR\s+([0-9]+)', jasl_common_lines).group(1)),
        'patch': int(re.search(
            r'#define\s+JASL_VERSION_PATCH\s+([0-9]+)', jasl_common_lines).group(1)),
        'label': re.search(
            r'#define\s+JASL_VERSION_LABEL\s+"(|(?:-[^"]+))"', jasl_common_lines).group(1),
        'date': re.search(
            r'#define\s+JASL_VERSION_DATE\s+"([^"]+)"', jasl_common_lines).group(1)
       }

    with open(args.changelog, 'r') as f:
        changelog_lines = ''.join(f.readlines())

    # example:## [0.1.0-beta] - 2018-04-12
    match = re.search(
        r'## \[([0-9]+)\.([0-9]+)\.([0-9]+)(|(?:-[^\]]+))\] - (\S+)', changelog_lines)

    changelog_version = {
        'major': int(match.group(1)),
        'minor': int(match.group(2)),
        'patch': int(match.group(3)),
        'label': match.group(4),
        'date': match.group(5)
    }

    with open(args.doxygen_config, 'r') as f:
        doxygen_lines = ''.join(f.readlines())

    # example:PROJECT_NUMBER         = [0.1.1] - 2018-05-27
    match = re.search(
        r'PROJECT_NUMBER\s*=\s*\[([0-9]+)\.([0-9]+)\.([0-9]+)(|(?:-[^\]]+))\] - (\S+)', doxygen_lines)
    assert(match)

    doxygen_version = {
        'major': int(match.group(1)),
        'minor': int(match.group(2)),
        'patch': int(match.group(3)),
        'label': match.group(4),
        'date': match.group(5)
    }

    compare(jasl_common_version, changelog_version)
    compare(jasl_common_version, doxygen_version)

    # touch
    open(args.output, 'w').close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    vc_parser = subparsers.add_parser("version-consistency")
    vc_parser.add_argument("--changelog", required=True)
    vc_parser.add_argument("--jasl_common", required=True)
    vc_parser.add_argument("--doxygen_config", required=True)
    vc_parser.add_argument("--output", required=True)
    vc_parser.set_defaults(func=version_consistency)

    args = parser.parse_args()
    args.func(args)

    sys.exit(0)
