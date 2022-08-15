#include <ctre/utf8.hpp>
#include <algorithm>

#ifdef CTRE_ENABLE_UTF8_RANGE

#define UNICODE_TEST(a) static_assert(call_test(u8 ##a, U ##a))

constexpr bool call_test(std::u8string_view a, std::u32string_view b) {
	auto utf = ctre::utf8_range(a);
	
	auto a_it = utf.begin();
	auto a_end = utf.end();
	
	auto b_it = b.begin();
	auto b_end = b.end();
	
	while (a_it != a_end && b_it != b_end) {
		if (*a_it != *b_it) return false;
		++a_it;
		++b_it;
	}
	
	if (a_it != a_end) return false;
	if (b_it != b_end) return false;
	
	return true;
}

#else

#define UNICODE_TEST(a) do { } while (false)

#endif

int main() {
	UNICODE_TEST("test");
	UNICODE_TEST("ěščř");
	UNICODE_TEST("Г");
	UNICODE_TEST("Гa");
	UNICODE_TEST("Гa😀");
	UNICODE_TEST("Гa😀ᦉ");
	UNICODE_TEST("Гa😀ᦉ🜇aa");
}