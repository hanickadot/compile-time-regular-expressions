# Compile time regular expressions v2

[![Build Status](https://travis-ci.org/hanickadot/compile-time-regular-expressions.svg?branch=master)](https://travis-ci.org/hanickadot/compile-time-regular-expressions)

Fast compile-time regular expression with support for matching/searching/capturing in compile-time or runtime.

You can use single header version from directory `single-header`. This header can be regenerated with `make single-header`.

More info at [compile-time.re](https://compile-time.re/)

## What this library can do?

```c++
ctre::match<"REGEX">(subject); // C++20
"REGEX"_ctre.match(subject); // C++17 + N3599 extension
```

* Matching
* Searching
* Capturing content (named captures are supported too)
* Back-Reference (\g{N} syntax, and \1...\9 syntax too)

The library is implementing most of the PCRE syntax with a few exceptions:

* atomic groups
* boundaries other than `^$`
* callouts
* character properties
* comments
* conditional patterns
* control characters (`\cX`)
* horizontal / vertical character classes (`\h\H\v\V`)
* match point reset (`\K`)
* named characters
* octal numbers
* options / modes
* subroutines
* unicode grapheme cluster (`\X`) 

More documentation on [pcre.org](https://www.pcre.org/current/doc/html/pcre2syntax.html). 

### What can be subject (input)?

* `std::string`-like object (`std::string_view` or your own string if it's providing `begin`/`end` functions with forward iterators)
* pair of forward iterators

## Supported compilers

* clang 6.0+ (template UDL, C++17 syntax)
* xcode clang 10.0+ (template UDL, C++17 syntax)
* gcc 7.4+ (template UDL, C++17 syntax)
* gcc 9.0+ (C++17 & C++20 cNTTP syntax)
* MSVC 15.8.8+ (C++17 syntax only)

#### Template UDL syntax

Compiler must support N3599 extension, as GNU extension in gcc (not in GCC 9.1+) and clang.

```c++
constexpr auto match(std::string_view sv) noexcept {
	using namespace ctre::literals;
	return "h.*"_ctre.match(sv);
}
```

If you need N3599 extension in GCC 9.1+ you can't use -pedantic mode and define macro `CTRE_ENABLE_LITERALS`.

#### C++17 syntax

You can provide pattern as a `constexpr ctll::fixed_string` variable.

```c++
static constexpr auto pattern = ctll::fixed_string{ "h.*" };

constexpr auto match(std::string_view sv) noexcept {
	return ctre::match<pattern>(sv);
}
```

(this is tested in MSVC 15.8.8)

#### C++20 syntax

Currently only compiler which supports cNTTP syntax `ctre::match<PATTERN>(subject)` is GCC 9+.

```c++
constexpr auto match(std::string_view sv) noexcept {
	return ctre::match<"h.*">(sv);
}
```

## Examples

#### Extracting number from input
```c++
std::optional<std::string_view> extract_number(std::string_view s) noexcept {
	if (auto m = ctre::match<"[a-z]+([0-9]+)">(s)) {
        return m.get<1>().to_view();
    } else {
        return std::nullopt;
    }
}
```
[link to compiler explorer](https://gcc.godbolt.org/z/5U67_e)


#### Extracting values from date
```c++
struct date { std::string_view year; std::string_view month; std::string_view day; };

std::optional<date> extract_date(std::string_view s) noexcept {
    using namespace ctre::literals;
    if (auto [whole, year, month, day] = ctre::match<"(\\d{4})/(\\d{1,2})/(\\d{1,2})">(s); whole) {
        return date{year, month, day};
    } else {
        return std::nullopt;
    }
}

//static_assert(extract_date("2018/08/27"sv).has_value());
//static_assert((*extract_date("2018/08/27"sv)).year == "2018"sv);
//static_assert((*extract_date("2018/08/27"sv)).month == "08"sv);
//static_assert((*extract_date("2018/08/27"sv)).day == "27"sv);
```
[link to compiler explorer](https://gcc.godbolt.org/z/x64CVp)

##### Using captures
```c++
auto result = ctre::match<"(?<year>\\d{4})/(?<month>\\d{1,2})/(?<day>\\d{1,2})">(s);
return date{result.get<"year">(), result.get<"month">, result.get<"day">};

// or in C++ emulation, but the object must have a linkage
static constexpr ctll::fixed_string year = "year";
static constexpr ctll::fixed_string month = "month";
static constexpr ctll::fixed_string day = "day";
return date{result.get<year>(), result.get<month>, result.get<day>};

// or use numbered access
// capture 0 is the whole match
return date{result.get<1>(), result.get<2>, result.get<3>};
```

#### Lexer
```c++
enum class type {
    unknown, identifier, number
};

struct lex_item {
    type t;
    std::string_view c;
};

std::optional<lex_item> lexer(std::string_view v) noexcept {
    if (auto [m,id,num] = ctre::match<"([a-z]+)|([0-9]+)">(v); m) {
        if (id) {
            return lex_item{type::identifier, id};
        } else if (num) {
            return lex_item{type::number, num};
        }
    }
    return std::nullopt;
}
```
[link to compiler explorer](https://gcc.godbolt.org/z/PKTiCC)

#### Range over input

This support is preliminary and probably the API will be changed.

```c++
auto input = "123,456,768"sv;

for (auto match: ctre::range<"([0-9]+),?">(input)) {
	std::cout << std::string_view{match.get<0>()} << "\n";
}
```
