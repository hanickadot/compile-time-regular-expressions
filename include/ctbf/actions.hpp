#ifndef CTRE__CTBF_ACTIONS__HPP
#define CTRE__CTBF_ACTIONS__HPP

#include "brainfuck.hpp"
#include <cstdint>

namespace ctbf {

/*
struct getchar: ctll::action {};
struct minus: ctll::action {};
struct plus: ctll::action {};
struct ptr_dec: ctll::action {};
struct ptr_inc: ctll::action {};
struct putchar: ctll::action {};
struct while_end: ctll::action {};
struct while_start: ctll::action {};
*/

template <typename... Sequence> struct sequence { };

struct empty;

struct value_inc { };
struct value_dec { };	
struct char_put { };
struct char_get { };
struct ptr_inc { };
struct ptr_dec { };
struct cycle_start { };

template <typename...> struct cycle { };


	
struct brainfuck_actions {
	// empty program
	template <auto V> static constexpr auto apply(brainfuck::make_program, ctll::term<V>, ctll::list<empty> stack) {
		return ctll::list<sequence<>>();
	}
	
	// non-empty program (expects only one thing on the stack)
	template <auto V, typename T> static constexpr auto apply(brainfuck::make_program, ctll::term<V>, ctll::list<T> stack) {
		return ctll::list<T>();
	}
	
	template <auto V, typename T, typename... Seq, typename... Ts> static constexpr auto apply(brainfuck::make_sequence, ctll::term<V>, ctll::list<T, sequence<Seq...>, Ts...> stack) {
		return ctll::list<sequence<Seq...,T>, Ts...>();
	}
	
template <auto V, typename A, typename B, typename... Ts> static constexpr auto apply(brainfuck::make_sequence, ctll::term<V>, ctll::list<A, B, Ts...> stack) {
	return ctll::list<sequence<B, A>, Ts...>();
}
	
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::minus, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<value_dec, Ts...>();
	}
	
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::plus, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<value_inc, Ts...>();
	}
	
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::ptr_dec, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<ptr_dec, Ts...>();
	}

	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::ptr_inc, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<ptr_inc, Ts...>();
	}
	
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::getchar, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<char_get, Ts...>();
	}
	
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::putchar, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<char_put, Ts...>();
	}
	
	// inserting empty mark
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::empty, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<empty, Ts...>();
	}
	
	// cycle start mark
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::while_start, ctll::term<V>, ctll::list<Ts...> stack) {
		return ctll::list<cycle_start, Ts...>();
	}

	// cycle with one item
	template <auto V, typename T, typename... Ts> static constexpr auto apply(brainfuck::while_end, ctll::term<V>, ctll::list<T, cycle_start, Ts...> stack) {
		return ctll::list<cycle<T>, Ts...>();
	}
	
	// cycle with one item (sequence)
	template <auto V, typename... Seq, typename... Ts> static constexpr auto apply(brainfuck::while_end, ctll::term<V>, ctll::list<sequence<Seq...>, cycle_start, Ts...> stack) {
		return ctll::list<cycle<Seq...>, Ts...>();
	}
	
	// empty cycle
	template <auto V, typename... Ts> static constexpr auto apply(brainfuck::while_end, ctll::term<V>, ctll::list<empty, cycle_start, Ts...> stack) {
		return ctll::list<cycle<>, Ts...>();
	}
};


};

#endif
