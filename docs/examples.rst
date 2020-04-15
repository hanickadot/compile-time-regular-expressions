Examples
========

Extracting a number from input
------------------------------
::

  std::optional<std::string_view> extract_number(std::string_view s) noexcept {
  	if (auto m = ctre::match<"[a-z]+([0-9]+)">(s)) {
          return m.get<1>().to_view();
      } else {
          return std::nullopt;
      }
  }

`link to compiler explorer <https://gcc.godbolt.org/z/5U67_e>`_

Extracting values from date
---------------------------
::


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

`link to compiler explorer <https://gcc.godbolt.org/z/x64CVp>`_

Lexer
-----
::

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

`link to compiler explorer <https://gcc.godbolt.org/z/PKTiCC>`_

Range over input
----------------

This support is preliminary and probably the API will be changed.

::

  auto input = "123,456,768"sv;
  
  for (auto match: ctre::range<"([0-9]+),?">(input)) {
  	std::cout << std::string_view{match.get<0>()} << "\n";
  }