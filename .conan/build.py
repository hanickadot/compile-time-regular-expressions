#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
from cpt.packager import ConanMultiPackager


if __name__ == "__main__":
    builder = ConanMultiPackager(
        username="crte",
        upload="https://api.bintray.com/conan/hanickadot/ctre",
        upload_only_when_stable=True,
        stable_branch_pattern="v?\d+\.\d+.*",
        test_folder=os.path.join(".conan", "test_package"))
    builder.add_common_builds()
    builder.run()
