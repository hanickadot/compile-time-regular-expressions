#!/usr/bin/env python
# -*- coding: utf-8 -*-
from conans import ConanFile


class CtreConan(ConanFile):
    name = "CTRE"
    version = "2.0"
    license = "MIT"
    url = "https://github.com/hanickadot/compile-time-regular-expressions.git"
    author = "Hana Dusíková (ctre@hanicka.net)"
    description = "Compile Time Regular Expression for C++17/20"
    homepage = "https://github.com/hanickadot/compile-time-regular-expressions"
    exports = "LICENSE"
    exports_sources = "include/*"
    no_copy_source = True
    
    def package(self):
        self.copy("*.hpp")
    
    def package_id(self):
        self.info.header_only()

