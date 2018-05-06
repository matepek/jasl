#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import io
import subprocess
import argparse
import re

# https://docs.microsoft.com/hu-hu/cpp/build/setting-the-path-and-environment-variables-for-command-line-builds

path_re = re.compile(r"(path)=(.*)", re.IGNORECASE)
environment_variables_re = [
    path_re,
    re.compile(r"(winddir|systemroot|devenvdir|extensionsdkdir" +
               r"|include|lib|libpath|ucrtversion"
               r"|universalcrtsdkdir|temp|tmp)=(.*)",
               re.IGNORECASE),
    re.compile(r"(framework[^=]*)=(.*)", re.IGNORECASE),
    re.compile(r"(vc[^=]*)=(.*)", re.IGNORECASE),
    re.compile(r"(visualstudio[^=]*)=(.*)", re.IGNORECASE),
    re.compile(r"(vs[^=]+)=(.*)", re.IGNORECASE),
    re.compile(r"(windows[^=]*)=(.*)", re.IGNORECASE),
    re.compile(r"(__[^=]+)=(.*)", re.IGNORECASE)
]
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    # [arch] [platform_type] [winsdk_version] [-vcvars_ver=vc_version]
    parser.add_argument("--arch", required=True)
    parser.add_argument("--vs-path", required=True)
    args = parser.parse_args()

    output = subprocess.check_output([
        args.vs_path,
        args.arch,
        "&&",
        "SET"
    ], shell=True, stderr=subprocess.STDOUT)

    envs = {}
    path = None
    for line in output.splitlines():
        for r in environment_variables_re:
            m = r.match(line)
            if m:
                envs[m.group(1)] = m.group(2)
                if r == path_re:
                    path = m.group(2)
                break

    env_filename = "env." + args.arch + ".txt"
    with io.open(env_filename, "w") as f:
        for k, v in envs.iteritems():
            f.write(k + u"=" + v + "\0")
        f.write(u"\0")

    tool_dir = None
    for p in path.split(os.pathsep):
        if os.path.exists(os.path.join(p, "cl.exe")):
            assert(os.path.exists(os.path.join(p, "link.exe")))
            tool_dir = p
            break
    assert(tool_dir)

    print('env_filename = "' + env_filename + '"')
    print('tool_dir = "' + tool_dir + '"')
