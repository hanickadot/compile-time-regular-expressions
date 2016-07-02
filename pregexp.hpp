#include "static-decider.hpp"
#include "static-regexp/string.hpp"
#include "static-regexp/static-regexp.hpp"

#include <iostream>
#include <cassert>

/*


backslash={\\}
dot={.}
dolar={$}
open={(}
close={)}
questionmark={?}
colon={:}
sopen={[}
sclose={]}
plus={+}
star={*}
copen={\{}
cclose={\}}
num={0,1,2,3,4,5,6,7,8,9}
comma={\,}
pipe={|}
caret={^}
minus={-}

hexdec={0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f}
octal={0,1,2,3,4,5,6,7}
nonspecial={a,b,c,d,e,f,x,:,],0,1,2,3,4,5,6,7,8,9,\,,-}
nonspecial2={a,b,c,d,e,f,x,:,0,1,2,3,4,5,6,7,8,9}

S-><alt> | epsilon

alt-><string>,<alt2>
altB-><string>,<alt2> | epsilon,[empty]
alt2->pipe,<alt>,[alt] | epsilon

string-><atom_repeat>,<string2>
string2-><atom_repeat>,<string2>,[create_sequence] | epsilon

atom_repeat-><atom>,<repeat>

atom-><char>|<nonchar>|<block>|<class>

repeat->epsilon | plus,[plus],<mod>|star,[star],<mod>|questionmark,[optional],<mod>
repeat->copen,<number>,[repeat_exactly],cclose
repeat->copen,<number>,comma,[repeat_at_least],cclose,<mod>
repeat->copen,<number>,comma,<number>,[repeat_ab],cclose,<mod>
mod->epsilon | questionmark,[nongreedy] | plus,[possessive]

number->[digit],num,<number2>,[number]
number2->epsilon | [digit],num,<number2>

block->open,<altB>,close | open,questionmark,colon,<altB>,close

class->sopen,[set_start],<set>,sclose,[set_finish]|sopen,caret,[set_neg_start],<set>,sclose,[set_finish]
set-><setitem>,<set2>
set2-><setitem>,[set_combine],<set2>|epsilon

setitem->[addchar],other,<range> | [addchar],nonspecial2,<range> | [addchar],dot,<range>
range->epsilon|minus,[range],other|minus,[range],nonspecial2

setitem->backslash,<backslash> | backslash,b,[backb] 

backslash->[addchar],<special>
special->dot|sopen|copen|cclose|open|close|backslash|star|plus|questionmark|pipe|caret|dolar

backslash->a,[backa]
backslash->x,[hdigit],hexdec,[hdigit],hexdec,[hexdec2]
backslash->x,copen,[hdigit],hexdec,[hdigit],hexdec,[hdigit],hexdec,[hdigit],hexdec,cclose,[hexdec4]
backslash->0,[odigit],octal,[odigit],octal,[odigit],octal,[octal3]
backslash_char->d,[class_digit]

char->[addchar],other | [addchar],nonspecial | dot,[any] | backslash,<backslash> | backslash,<backslash_char>
nonchar->caret,[insert_begin] | dolar,[insert_end]

*/

#define LET_IT_GO template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<Content...>{}; }

struct insert_begin { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::Begin, Content...>{}; } };

struct insert_end { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::End, Content...>{}; } };

struct create_sequence { 
	template <char c, typename A, typename B, typename ... Content> static constexpr auto build(const Static::Stack<A, B, Content...> &&) { return Static::Stack<sre::Sequence<B,A>, Content...>{}; }
	template <char c, typename A, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::Sequence<Seq...>, A, Content...> &&) { return Static::Stack<sre::Sequence<A, Seq...>, Content...>{}; }
	LET_IT_GO
};

template <bool positive> struct SetSeed { };

struct set_start { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<SetSeed<true>,Content...>{}; } };
struct set_neg_start { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<SetSeed<false>,Content...>{}; } };

struct set_finish { 
	LET_IT_GO
};

struct set_combine { 
	template <char c, unsigned int b, unsigned int a, typename ... Content> static constexpr auto build(const Static::Stack<sre::Char<b>, sre::Char<a>, Content...> &&) { return Static::Stack<sre::Char<a,b>, Content...>{}; }
	template <char c, unsigned int a, unsigned int ... def, typename ... Content> static constexpr auto build(const Static::Stack<sre::Range<def...>, sre::Char<a>, Content...> &&) { return Static::Stack<sre::Range<def...,a,a>, Content...>{}; }
	template <char c, unsigned int ... b, unsigned int ... a, typename ... Content> static constexpr auto build(const Static::Stack<sre::Range<b...>,sre::Range<a...>,Content...> &&) { return Static::Stack<sre::Range<a...,b...>, Content...>{}; }
	template <char c, unsigned int b, unsigned int a, typename ... Content> static constexpr auto build(const Static::Stack<sre::Char<b>, sre::NegChar<a>, Content...> &&) { return Static::Stack<sre::NegChar<a,b>, Content...>{}; }
	template <char c, unsigned int a, unsigned int ... def, typename ... Content> static constexpr auto build(const Static::Stack<sre::Range<def...>, sre::NegChar<a>, Content...> &&) { return Static::Stack<sre::NegativeRange<def...,a,a>, Content...>{}; }
	template <char c, unsigned int ... b, unsigned int ... a, typename ... Content> static constexpr auto build(const Static::Stack<sre::Range<b...>,sre::NegativeRange<a...>,Content...> &&) { return Static::Stack<sre::NegativeRange<a...,b...>, Content...>{}; }
	LET_IT_GO
};

struct range { 
	template <char last, unsigned int first, typename ... Content> static constexpr auto build(const Static::Stack<sre::Char<first>, Content...> &&) { return Static::Stack<sre::Range<first,last>, Content...>{}; }
	template <char last, unsigned int first, typename ... Content> static constexpr auto build(const Static::Stack<sre::NegChar<first>, Content...> &&) { return Static::Stack<sre::NegativeRange<first,last>, Content...>{}; }
	LET_IT_GO
};

struct addchar { 
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::Char<c>, Content...>{}; }
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<SetSeed<true>,Content...> &&) { return Static::Stack<sre::Char<c>, Content...>{}; }
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<SetSeed<false>,Content...> &&) { return Static::Stack<sre::NegChar<c>, Content...>{}; }
};
	 

struct optional { 
	template <char c, typename A, typename ... Content> static constexpr auto build(const Static::Stack<A, Content...> &&) { return Static::Stack<sre::Optional<A>, Content...>{}; }
	template <char c, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::Sequence<Seq...>, Content...> &&) { return Static::Stack<sre::Optional<Seq...>, Content...>{}; }
	LET_IT_GO
};

struct plus { 
	template <char c, typename A, typename ... Content> static constexpr auto build(const Static::Stack<A, Content...> &&) { return Static::Stack<sre::Plus<A>, Content...>{}; }
	template <char c, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::Sequence<Seq...>, Content...> &&) { return Static::Stack<sre::Plus<Seq...>, Content...>{}; }
	LET_IT_GO
};

struct star { 
	template <char c, typename A, typename ... Content> static constexpr auto build(const Static::Stack<A, Content...> &&) { return Static::Stack<sre::Star<A>, Content...>{}; }
	template <char c, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::Sequence<Seq...>, Content...> &&) { return Static::Stack<sre::Star<Seq...>, Content...>{}; }
	LET_IT_GO
};

template <unsigned int> struct DigitValue { };

struct digit { 
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<DigitValue<c-'0'>, Content...>{}; }
	template <char c, unsigned int i, typename ... Content> static constexpr auto build(const Static::Stack<DigitValue<i>,Content...> &&) { return Static::Stack<DigitValue<i*10 + (c-'0')>, Content...>{}; }
};

template <unsigned int> struct NumberValue { };

struct number { 
	template <char c, unsigned int i, typename ... Content> static constexpr auto build(const Static::Stack<DigitValue<i>,Content...> &&) { return Static::Stack<NumberValue<i>, Content...>{}; }
	LET_IT_GO
};

struct repeat_exactly {
	template <char c, unsigned int i, typename Subject, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<i>,Subject,Content...> &&) { return Static::Stack<sre::RepeatHelper<i,i,true,Subject>, Content...>{}; }
	template <char c, unsigned int i, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<i>,sre::Sequence<Seq...>,Content...> &&) { return Static::Stack<sre::RepeatHelper<i,i,true,Seq...>, Content...>{}; }
	LET_IT_GO
};

struct repeat_at_least { 
	template <char c, unsigned int i, typename Subject, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<i>,Subject,Content...> &&) { return Static::Stack<sre::RepeatHelper<i,0,true,Subject>, Content...>{}; }
	template <char c, unsigned int i, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<i>,sre::Sequence<Seq...>,Content...> &&) { return Static::Stack<sre::RepeatHelper<i,0,true,Seq...>, Content...>{}; }
	LET_IT_GO
};

struct repeat_ab { 
	template <char c, unsigned int a, unsigned int b, typename Subject, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<b>,NumberValue<a>,Subject,Content...> &&) { return Static::Stack<sre::RepeatHelper<a,b,true,Subject>, Content...>{}; }
	template <char c, unsigned int a, unsigned int b, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<NumberValue<b>,NumberValue<a>,sre::Sequence<Seq...>,Content...> &&) { return Static::Stack<sre::RepeatHelper<a,b,true,Seq...>, Content...>{}; }
	LET_IT_GO
};

struct nongreedy {
	template <char c, unsigned int a, unsigned int b, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::RepeatHelper<a,b,true,Seq...>,Content...> &&) { return Static::Stack<sre::RepeatHelper<a,b,false,Seq...>,Content...>{}; }
	LET_IT_GO
};
struct possessive { LET_IT_GO }; // unimplemented

struct alt { 
	template <char c, typename A, typename B, typename ... Content> static constexpr auto build(const Static::Stack<A, B, Content...> &&) { return Static::Stack<sre::Select<B,A>, Content...>{}; }
	template <char c, typename A, typename ... Seq, typename ... Content> static constexpr auto build(const Static::Stack<sre::Select<Seq...>, A, Content...> &&) { return Static::Stack<sre::Select<A, Seq...>, Content...>{}; }
	LET_IT_GO
};

struct any { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::Anything, Content...>{}; } };

struct empty { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::Empty, Content...>{}; } };

struct class_digit { template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) { return Static::Stack<sre::Number, Content...>{}; } };

struct backa { LET_IT_GO };
struct backb { LET_IT_GO };


struct hdigit { LET_IT_GO };
struct odigit { LET_IT_GO };
struct octal3 { LET_IT_GO };
struct hexdec2 { LET_IT_GO };
struct hexdec4 { LET_IT_GO };





#include "pregexp-grammar.hpp"

template <template<typename...> typename Base> struct RegExpReturnTypeHelper {
	template <typename... Content> static constexpr auto transform(const sre::Sequence<Content...> &&) {
		return Base<Content...>{};
	}
	template <typename Content> static constexpr auto transform(const Content &&) {
		return Base<Content>{};
	}
};

template<typename CharT, CharT ... string> constexpr auto operator""_pre() {
	static_assert(std::is_same<CharT, char>::value);
	using namespace Static;
	using Parser = Decider<Stack<Grammar::NT_S>, Input<string...>>;
	static_assert(Parser::correct);
	return RegExpReturnTypeHelper<sre::RegExp>::transform(typename Parser::template Type<>{});
}

template<typename CharT, CharT ... string> constexpr auto operator""_fpre() {
	static_assert(std::is_same<CharT, char>::value);
	using namespace Static;
	using Parser = Decider<Stack<Grammar::NT_S>, Input<string...>>;
	static_assert(Parser::correct);
	return RegExpReturnTypeHelper<sre::FloatingRegExp>::transform(typename Parser::template Type<>{});
}

template <typename T, typename A> constexpr void checkType(const A &&) {
	static_assert(std::is_same<T,A>::value);
} 
