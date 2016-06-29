#include <iostream>
 
template <char... string> struct ConvertToDouble;
 
template <> struct ConvertToDouble<> {
    static constexpr unsigned int exp = 0;
    static constexpr double value = 0;
    static constexpr double value2(double) { return 0; }
};
 
template <char first, char... rest> struct ConvertToDouble<first,rest...> {
    static_assert(first >= '0' && first <= '9');
    static constexpr double exp = ConvertToDouble<rest...>::exp ? 10 * ConvertToDouble<rest...>::exp : 1;
    static constexpr double value = (first - '0') * exp + ConvertToDouble<rest...>::value;
    static constexpr double value2(double exp2) {
        return (first - '0') / exp2 + ConvertToDouble<rest...>::value2(exp2*10.0);
    }
};
 
template <char... rest> struct ConvertToDouble<'.',rest...> {
    static constexpr unsigned int exp = 0;
    static constexpr double value = ConvertToDouble<rest...>::value2(10.0);
};
 
template <char... rest> struct ConvertToDouble<'-',rest...> {
    static constexpr unsigned int exp = 0;
    static constexpr double value = - ConvertToDouble<rest...>::value;
};
 
template <uint16_t constant, char ... string> struct MyNumber {
	constexpr auto operator-() const {
		return MyNumber<constant,'-',string...>{};
	}
	constexpr operator int16_t() const {
		static_assert(ConvertToDouble<string...>::value >= -1, "Constant is not bigger than -1");
		static_assert(ConvertToDouble<string...>::value < 1, "Constant is not lesser than 1");
		return constant * ConvertToDouble<string...>::value;
	}
}; 
 
 
template <char... string> constexpr auto operator "" _q15() {
    return MyNumber<1 << 15, string...>{};
}
 
constexpr int16_t Q15(float a) { return (int16_t)(a * 32768 );}
 
int main() {
	std::cout << (-1_q15) << " vs " << Q15(-1) << "\n";
	std::cout << (0.99_q15) << " vs " << Q15(0.99) << "\n";
	std::cout << (0.25_q15) << " vs " << Q15(0.25) << "\n";
	std::cout << (0_q15) << " vs " << Q15(0) << "\n";
	std::cout << (-0.2_q15) << " vs " << Q15(-0.2) << "\n";
}