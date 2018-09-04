#include <ctbf.hpp>

using namespace ctbf::literals;

using namespace ctbf::test_literals;

#define DEBUG

#ifdef DEBUG

template <typename... T> struct id_type;

template <typename... T> constexpr auto id(T...) {
	return id_type<T...>{};
};

#define ID(expr) auto i = id(expr);

#endif // DEBUG

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(ctbf::code<A>, A) -> std::true_type { return {}; }

template <typename A, typename B> constexpr bool same_f(A a,B b) {
	return same(ctll::front(a), b);
}

using namespace ctbf;

static_assert("-+"_ctbf_syntax);
static_assert("[->+<]"_ctbf_syntax);
static_assert("[->abc+<]"_ctbf_syntax);
static_assert("[]"_ctbf_syntax);
static_assert("[][]"_ctbf_syntax);
static_assert("[[]][]"_ctbf_syntax);

//ID("+-+"_ctbf);
static_assert(same("+-"_ctbf, sequence<value_inc, value_dec>()));
static_assert(same("+-+"_ctbf, sequence<value_inc, value_dec, value_inc>()));
static_assert(same("a+b-c+d"_ctbf, sequence<value_inc, value_dec, value_inc>()));
static_assert(same("+-[+]"_ctbf, sequence<value_inc, value_dec, cycle<value_inc>>()));
static_assert(same("+-[]"_ctbf, sequence<value_inc, value_dec, cycle<>>()));
static_assert(same("+-[[[]]]"_ctbf, sequence<value_inc, value_dec, cycle<cycle<cycle<>>>>()));
static_assert(same("[->+<]"_ctbf, cycle<value_dec, ptr_inc, value_inc, ptr_dec>()));


//static_assert("[[[]]][[[]]]"_ctbf_syntax);
//static_assert("[a[b[c]d]e]f[g[h[i]j]k]"_ctbf_syntax);

