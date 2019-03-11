#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from conans import ConanFile


class CtreConan(ConanFile):
    name = "CTRE"
    license = "Apache 2.0 with LLVM Exception"
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

