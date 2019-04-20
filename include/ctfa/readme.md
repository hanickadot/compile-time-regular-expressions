# CTFA
_Compile Time Finite Automaton_ library by Hana Dusíková (2019)


## Example

### Match
```
static constexpr auto aloha = ctfa::basic::string<'a','l','o','h','a'>;
static constexpr auto ahoj = ctfa::basic::string<'a','h','o','j'>;

static constexpr auto combined = ctfa::minimize<ctfa::determinize<ctfa::alternative<aloha, ahoj>>>;

bool match(std::string_view sv) {
	return ctfa::match<combined>(sv);
}
```

### Search
```
static constexpr auto hana = ctfa::basic::string<'h','a','n','a'>;
static constexpr auto hana_anywhere = ctfa::concat<ctfa::star<ctfa::basic::anything>, hana, ctfa::star<ctfa::basic::anything>>;

static constexpr auto optimized = ctfa::minimize<ctfa::determinize<hana_anywhere>>;

bool search_for_hana(std::string_view sv) {
	return ctfa::match<optimized>(sv);
}
```

## Algorithmic guarantees

Matching is O(n) independently on FA complexity (n = length of the subject).

## Supported compilers

The library was tested on:

* clang 8
* gcc 9

But it should need just any C++17 compliant compiler.

