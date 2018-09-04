#include <ctbf.hpp>

using namespace ctbf::literals;

int main() {
	R"bf(
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
	)bf"_ctbf.eval<20>();
}