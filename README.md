# Compile time regular expressions v2

[![Build Status](https://travis-ci.org/hanickadot/compile-time-regular-expressions.svg?branch=master)](https://travis-ci.org/hanickadot/compile-time-regular-expressions)

Fast compile-time regular expression with support for matching/searching/capturing in compile-time or runtime.

## What this library can do?

```c++
ctre::match<"REGEX">(subject); // C++20
"REGEX"_ctre.match(subject); // C++17 + N3599 extension
```

* Matching
* Searching
* Capturing content (named captures are supported too)

### What can be subject (input)?

* `std::string`-like object (`std::string_view` or your own string if it's providing `begin`/`end` functions with forward iterators)
* pair of forward iterators

## Examples

#### Extracting number from input
```c++
std::optional<std::string_view> extract_number(std::string_view s) noexcept {
    using namespace ctre::literals;
    if (auto m = "^[a-z]++([0-9]++)$"_ctre.match(s)) {
        return m.get<1>().to_view();
    } else {
        return std::nullopt;
    }
}
```
[link to compiler explorer](https://godbolt.org/z/xi5ulD)


#### Extracting values from date
```c++
struct date { std::string_view year; std::string_view month; std::string_view day; };

constexpr std::optional<date> extract_date(std::string_view s) noexcept {
    using namespace ctre::literals;
    if (auto [whole, year, month, day] = "^([0-9]{4})/([0-9]{1,2}+)/([0-9]{1,2}+)$"_ctre.match(s); whole
    ) {
        return date{year.to_view(), month.to_view(), day.to_view()};
    } else {
        return std::nullopt;
    }
}

static_assert(extract_date("2018/08/27"sv).has_value());
static_assert((*extract_date("2018/08/27"sv)).year == "2018"sv);
static_assert((*extract_date("2018/08/27"sv)).month == "08"sv);
static_assert((*extract_date("2018/08/27"sv)).day == "27"sv);
```
[link to compiler explorer](https://godbolt.org/z/QJ6Ecb)

#### Lexer
```c++
enum class type {
    unknown, identifier, number
};

struct lex_item {
    type t;
    std::string_view c;
};

constexpr std::optional<lex_item> lexer(std::string_view v) noexcept {
    using namespace ctre::literals;
    if (auto [m,id,num] = "^([a-z]++)|([0-9]++)$"_ctre.match(v); m) {
        if (id) {
            return lex_item{type::identifier, id};
        } else if (num) {
            return lex_item{type::number, num};
        }
    }
    return std::nullopt;
}
```
[link to compiler explorer](https://godbolt.org/z/iSgFiK)

## Supported compilers

* clang 5.0+
* gcc 7.2+

Compiler must support N3599 extension (as GNU extension in gcc and clang) or C++20 class NTTP (P0732).
