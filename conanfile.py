#!/usr/bin/env python
# -*- coding: utf-8 -*-
from conans import ConanFile


class CtreConan(ConanFile):
    name = "CTRE"
    version = "2.1"
    license = "MIT"
    url = "https://github.com/hanickadot/compile-time-regular-expressions"
    author = "Hana Dusíková (ctre@hanicka.net)"
    description = "Compile Time Regular Expression for C++17/20"
    homepage = "https://github.com/hanickadot/compile-time-regular-expressions"
    no_copy_source = True
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    
    def package(self):
        self.copy("LICENSE", "licenses")
        self.copy("*.hpp")
    
    def package_id(self):
        self.info.header_only()

