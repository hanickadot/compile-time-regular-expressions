ctre::reflect (experimental)
============================

``ctre::reflect`` unpacks regex captures directly into the fields of a struct, using C++26 static reflection (P2996). Instead of pulling groups out of a match object by index or name, you describe the shape you want as a plain struct and get its fields populated by the library.

Experimental. Requires a C++26 compiler with P2996 reflection support (build with ``-freflection``). ``CTRE_SUPPORTS_CPP26_REFLECTION`` is automatically defined when the compiler sets ``__cpp_impl_reflection >= 202603L``; the reflect API is a no-op otherwise. I basically tested it only with GCC 16.

.. code-block:: cpp

  #include <ctre-reflect.hpp>
  #include <optional>

  struct Url
  {
      std::string_view scheme = "http";
      std::string_view host;
      std::optional<std::uint16_t> port;
      std::string_view path = "/";
  };

  constexpr auto url_matcher = ctre::reflect::match<Url, "(?:(http|https)://)?([a-z0-9.]+)(?::([0-9]+))?(.*)?">;

  constexpr auto u = url_matcher("localhost:3000");
  static_assert(u && u->scheme == "http" && u->host == "localhost" && u->port == 3000 && u->path == "/");

  constexpr auto u2 = url_matcher("https://google.com/oopsie");
  static_assert(u2 && u2->scheme == "https" && u2->host == "google.com"  && !u2->port && u2->path == "/oopsie");

  constexpr int s = [] {
      int sum = 0;
      struct kv { std::string_view key; int val; };
      for (auto m : ctre::reflect::search_all<kv, "(\\w+)=(\\d+)">("A=1 B=2 J=10")) sum += m.val;
      return sum;
  }();
  static_assert(s == 13);


API
------------
In ``ctre::reflect::`` namespace: ``match``, ``search``, ``starts_with``, ``search_all``, ``tokenize`` and the ``multiline_`` versions as well.

Follow ``ctre::`` wrappers' API, taking the struct type to unpack to as extra first template argument. That type is constrained by the ``ctre::reflectable`` concept (an aggregate class) — types with user-provided constructors, private or virtual members, etc. are rejected.

How it works
------------

You pass a regex and a struct type. Each capture group, if its value is not empty,
gets assigned to a field.

- If all capture groups are named (``(?<name>...)``), field order in the struct is not important. A named capture group whose name has no matching field in the struct is a compile error.
- Otherwise, captures are assigned by position. More capture groups than struct fields is a compile error. If some groups are named, their name must match the field name at the same position (or you get a compile error).
- Extra fields left default-constructed.
- Optional-like fields are supported.
- Numerical fields (any arithmetic type except ``bool``) are converted using ``std::from_chars``. **Conversion failure fails the entire match** for ``match``/``search``/``starts_with`` (returns ``std::nullopt``). For ``search_all``/``tokenize``, items that fail conversion are silently dropped from the range. (TODO: Add conversion customization point.)
- ``bool`` fields are not parsed as text: a non-empty capture is ``true``, an empty/absent capture is ``false`` (same as the general capture-to-bool cast). The standard has no lightweight constexpr-friendly "true"/"false" parser.
- Capture group 0 (whole-match) is not accessible.

Split operation doesn't make sense with the ``ctre::reflect`` API.