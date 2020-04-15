ctre
====

A compile-time (almost) PCRE-compatible regular expression matcher for C++.

Overview
========

Fast compile-time regular expressions with support for matching/searching/capturing at compile-time or runtime. ::

  ctre::match<"REGEX">(subject); // C++20
  "REGEX"_ctre.match(subject); // C++17 + N3599 extension

.. toctree::
   :maxdepth: 2

   api
   examples
   regex_syntax

Supported compilers
===================

- clang 6.0+ (template UDL, C++17 syntax)
- xcode clang 10.0+ (template UDL, C++17 syntax)
- gcc 7.4+ (template UDL, C++17 syntax)
- gcc 9.0+ (C++17 & C++20 cNTTP syntax)
- MSVC 15.8.8+ (C++17 syntax only)

Basic Usage
===========

Template UDL syntax
-------------------

Compiler must support N3599 extension, as GNU extension in gcc (not in GCC 9.1+) and clang. ::

  constexpr auto match(std::string_view sv) noexcept {
  	using namespace ctre::literals;
  	return "h.*"_ctre.match(sv);
  }

If you need N3599 extension in GCC 9.1+ you can't use -pedantic mode and define the macro ``CTRE_ENABLE_LITERALS``.

C++17 syntax
------------

You can provide pattern as a constexpr ``ctll::fixed_string variable``. ::

  static constexpr auto pattern = ctll::fixed_string{ "h.*" };
  
  constexpr auto match(std::string_view sv) noexcept {
  	return ctre::match<pattern>(sv);
  }

(this is tested in MSVC 15.8.8)

C++20 syntax
------------

Currently only compiler which supports cNTTP syntax ``ctre::match<PATTERN>(subject)`` is GCC 9+. ::

  constexpr auto match(std::string_view sv) noexcept {
  	return ctre::match<"h.*">(sv);
  }

