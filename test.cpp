#include "static-stack.hpp"
#include "static-decider.hpp"

#include <type_traits>
#include <iostream>
#include <cstdint>

struct A {
	int a;
	constexpr bool operator==(const A & right) const {
		return a == right.a;
	}
};

std::ostream & operator<<(std::ostream & stream, const A & obj) {
	return stream << obj.a;
} 

struct B {
	int b;
	constexpr bool operator==(const B & right) const {
		return b == right.b;
	}
};


std::ostream & operator<<(std::ostream & stream, const B & obj) {
	return stream << obj.b;
} 

using namespace Static;


int main() {
	
	Stack<B,A> stack_aa{{1},{2}};
	Stack<A,B,A> stack_aaa{{3},{1},{2}};
	
	stack_aaa.template nearest<B>().b = 42;
	
	auto b = stack_aa.push(Stack<A,B>{{3},{4}});
	//auto c = stack_aa.push(Stack<A>{{3}});
	//static_assert(std::is_same<decltype(b),decltype(stack_aaa)>::value);
	////static_assert(std::is_same<decltype(c),decltype(stack_aaa)>::value);
	//auto d = stack_aaa.pop();
	//static_assert(std::is_same<decltype(d),decltype(stack_aa)>::value);
	
	
	//std::cout << "stack_aa: " << stack_aa << "\n";
	//std::cout << "stack_aaa: " << stack_aaa << "\n";
	//std::cout << "b: " << b << "\n";
	//std::cout << "d: " << d << "\n";
	
	//static_assert(stack_aaa == b, "not same");
	
}