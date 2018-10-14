#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
from cpt.packager import ConanMultiPackager


def get_travis_branch():
    return os.getenv("TRAVIS_BRANCH")


def get_reference():
    return "CTRE/{}".format(get_travis_branch())


if __name__ == "__main__":
    builder = ConanMultiPackager(
        reference=get_reference(),
        username="ctre",
        upload="https://api.bintray.com/conan/hanickadot/ctre",
        upload_only_when_stable=True,
        stable_branch_pattern="v?\d+\.\d+.*",
        test_folder=os.path.join(".conan", "test_package"))
    builder.add_common_builds()
    builder.run()
