#ifndef CTRE_STD_MODULE_HPP
#define CTRE_STD_MODULE_HPP

#if __cpp_lib_modules >= 202207L
import std;
#else

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#if __has_include(<charconv>)
#include <charconv>
#endif

#endif

#define CTRE_IN_MODULE
#define CTLL_IN_MODULE

#endif
