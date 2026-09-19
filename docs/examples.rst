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

Iterating over all matches
--------------------------

``ctre::search_all`` searches forward from the end of the previous match and
yields one result per match, skipping whatever lies between them. ::

  auto input = "123,456,768"sv;

  for (auto match: ctre::search_all<"[0-9]+">(input)) {
  	std::cout << std::string_view{match.get<0>()} << "\n";
  }

  // 123
  // 456
  // 768

``ctre::tokenize`` is the anchored counterpart. Every match has to start where
the previous one ended, so the range ends at the first character the pattern
does not accept rather than skipping over it. ::

  for (auto token: ctre::tokenize<"[a-z]+">("ab!!cd"sv)) {
  	std::cout << std::string_view{token.get<0>()} << "\n";
  }

  // ab

  // "cd" is never reached, because "!!" is not part of a token and tokenize
  // does not skip ahead. ctre::search_all with the same pattern and input
  // yields both "ab" and "cd".

``ctre::split`` yields the pieces between the matches instead of the matches
themselves. ::

  for (auto piece: ctre::split<",">("alpha,beta,gamma"sv)) {
  	std::cout << std::string_view{piece.get<0>()} << "\n";
  }

  // alpha
  // beta
  // gamma

All three also take the input on the left of ``operator|``. ::

  for (auto match: input | ctre::search_all<"[0-9]+">) {
  	std::cout << std::string_view{match.get<0>()} << "\n";
  }

``multiline_search_all``, ``multiline_tokenize`` and ``multiline_split`` are the
same three with the multiline modifier applied.

.. note::

  ``ctre::range`` is the former name of ``ctre::search_all`` and is deprecated,
  as is ``ctre::multiline_range`` in favour of ``ctre::multiline_search_all``.
