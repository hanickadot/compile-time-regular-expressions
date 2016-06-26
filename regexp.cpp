#include "static-parser.hpp"
#include <cstdio>

//  alpha={a}
//  num={1}
//  
//  S-><selection>,<S>|epsilon
//  selection-><part>,<selection2>
//  selection2->\|,<part>,<selection2>|epsilon
//  part-><content>,<modifier>
//  content->alpha | <set> | dot | <block> | <begin> | <end>
//  begin->^
//  end->$
//  modifier->? | + | \* | epsilon | {,<number>,} | {,<number>,\,,} | {,\,,<number>,}
//  number->num,<number2>
//  number2->num,<number2>|epsilon
//  block->(,<S>,)
//  set->\[,<pset>,\] | \[,^,<nset>,\]
//  pset->alpha,<pset>|epsilon
//  nset->alpha,<nset>|epsilon

namespace Regexp {
	struct nt_S{};
	struct nt_A{};
	struct nt_B{};
	struct nt_begin{};
	struct nt_block{};
	struct nt_C{};
	struct nt_content{};
	struct nt_end{};
	struct nt_modifier{};
	struct nt_nset{};
	struct nt_number2{};
	struct nt_number{};
	struct nt_part{};
	struct nt_pset{};
	struct nt_selection2{};
	struct nt_selection{};
	struct nt_set{};
	
	struct c_dolar{};
	struct c_open{};
	struct c_close{};
	struct c_star{};
	struct c_plus{};
	struct c_comma{};
	struct c_questionmark{};
	struct c_sopen{};
	struct c_sclose{};
	struct c_caret{};
	struct c_alpha{};
	struct c_dot{};
	struct c_copen{};
	struct c_pipe{};
	struct c_cclose{};
	struct c_num{};
}

using namespace Regexp;

#define MOVE(a,b) template <> struct Syntax::Move<Regexp::a, b> 

MOVE(nt_S,'$') { using N = Syntax::String<nt_selection, nt_S>; };
MOVE(nt_S,'(') { using N = Syntax::String<nt_selection, nt_S>; };
MOVE(nt_S,')') { using N = Epsilon; };
MOVE(nt_S,'*') { using N = RejectInput; };
MOVE(nt_S,'+') { using N = RejectInput; };
MOVE(nt_S,',') { using N = RejectInput; };
MOVE(nt_S,'?') { using N = RejectInput; };
MOVE(nt_S,'[') { using N = Syntax::String<nt_selection, nt_S>; };
MOVE(nt_S,']') { using N = RejectInput; };
MOVE(nt_S,'^') { using N = Syntax::String<nt_selection, nt_S>; };
MOVE(nt_S,'a') { using N = Syntax::String<nt_selection, nt_S>; };
MOVE(nt_S,'{') { using N = RejectInput; };
MOVE(nt_S,'|') { using N = RejectInput; };
MOVE(nt_S,'}') { using N = RejectInput; };
MOVE(nt_S,'.') { using N = Syntax::String<nt_selection, nt_S>; };
template <char c> struct Syntax::Move<nt_S,c> { using N = Syntax::String<nt_selection, nt_S>; };
template <> struct Syntax::Move<nt_S> { using N = Epsilon; };

MOVE(nt_A,',') { using N = Syntax::String<c_comma, nt_number, c_cclose>; };
MOVE(nt_A,'0') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'1') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'2') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'3') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'4') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'5') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'6') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'7') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'8') { using N = Syntax::String<nt_number, nt_C>; };
MOVE(nt_A,'9') { using N = Syntax::String<nt_number, nt_C>; };

MOVE(nt_B,'$') { using N = RejectInput; };
MOVE(nt_B,'(') { using N = RejectInput; };
MOVE(nt_B,')') { using N = RejectInput; };
MOVE(nt_B,'*') { using N = RejectInput; };
MOVE(nt_B,'+') { using N = RejectInput; };
MOVE(nt_B,',') { using N = RejectInput; };
MOVE(nt_B,'?') { using N = RejectInput; };
MOVE(nt_B,'[') { using N = RejectInput; };
MOVE(nt_B,'.') { using N = RejectInput; };
MOVE(nt_B,'{') { using N = RejectInput; };
MOVE(nt_B,'|') { using N = RejectInput; };
MOVE(nt_B,'}') { using N = RejectInput; };
MOVE(nt_B,']') { using N = Syntax::String<nt_pset, c_sclose>; };
MOVE(nt_B,'^') { using N = Syntax::String<c_caret, nt_nset, c_sclose>; };
template <char c> struct Syntax::Move<nt_B,c> { using N = Syntax::String<nt_pset, c_sclose>; };

MOVE(nt_begin,'^') { using N = Syntax::String<c_caret>; };
MOVE(nt_block,'(') { using N = Syntax::String<c_open,nt_S,c_close>; };
MOVE(nt_C,',') { using N = Syntax::String<c_comma,c_cclose>; };
MOVE(nt_C,'}') { using N = Syntax::String<c_cclose>; };

MOVE(nt_content,'$') { using N = Syntax::String<nt_end>; };
MOVE(nt_content,'(') { using N = Syntax::String<nt_block>; };
MOVE(nt_content,'[') { using N = Syntax::String<nt_set>; };
MOVE(nt_content,'^') { using N = Syntax::String<nt_begin>; };
MOVE(nt_content,'.') { using N = Syntax::String<c_dot>; };
MOVE(nt_content,')') { using N = RejectInput; };
MOVE(nt_content,'*') { using N = RejectInput; };
MOVE(nt_content,'+') { using N = RejectInput; };
MOVE(nt_content,',') { using N = RejectInput; };
MOVE(nt_content,'?') { using N = RejectInput; };
MOVE(nt_content,']') { using N = RejectInput; };
MOVE(nt_content,'{') { using N = RejectInput; };
MOVE(nt_content,'|') { using N = RejectInput; };
MOVE(nt_content,'}') { using N = RejectInput; };
template <char c> struct Syntax::Move<nt_content,c> { using N = Syntax::String<c_alpha>; };

MOVE(nt_end,'$') { using N = Syntax::String<c_dolar>; };

template <char c> struct Syntax::Move<nt_modifier,c> { using N = Epsilon; };
MOVE(nt_modifier,'*') { using N = Syntax::String<c_star>; };
MOVE(nt_modifier,'+') { using N = Syntax::String<c_plus>; };
MOVE(nt_modifier,',') { using N = RejectInput; };
MOVE(nt_modifier,'?') { using N = Syntax::String<c_questionmark>; };
MOVE(nt_modifier,']') { using N = RejectInput; };
MOVE(nt_modifier,'}') { using N = RejectInput; };
MOVE(nt_modifier,'{') { using N = Syntax::String<c_copen, nt_A>; };
template <> struct Syntax::Move<nt_modifier> { using N = Epsilon; };

MOVE(nt_nset,']') { using N = Epsilon; };
MOVE(nt_nset,'$') { using N = RejectInput; };
MOVE(nt_nset,'(') { using N = RejectInput; };
MOVE(nt_nset,')') { using N = RejectInput; };
MOVE(nt_nset,'*') { using N = RejectInput; };
MOVE(nt_nset,'+') { using N = RejectInput; };
MOVE(nt_nset,',') { using N = RejectInput; };
MOVE(nt_nset,'?') { using N = RejectInput; };
MOVE(nt_nset,'[') { using N = RejectInput; };
MOVE(nt_nset,'^') { using N = RejectInput; };
MOVE(nt_nset,'.') { using N = RejectInput; };
MOVE(nt_nset,'{') { using N = RejectInput; };
MOVE(nt_nset,'|') { using N = RejectInput; };
MOVE(nt_nset,'}') { using N = RejectInput; };
template <char c> struct Syntax::Move<nt_nset,c> { using N = Syntax::String<c_alpha, nt_nset>; };

MOVE(nt_number2,',') { using N = Epsilon; };
MOVE(nt_number2,'}') { using N = Epsilon; };
MOVE(nt_number2,'0') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'1') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'2') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'3') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'4') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'5') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'6') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'7') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'8') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number2,'9') { using N = Syntax::String<c_num, nt_number2>; };

MOVE(nt_number,'0') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'1') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'2') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'3') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'4') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'5') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'6') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'7') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'8') { using N = Syntax::String<c_num, nt_number2>; };
MOVE(nt_number,'9') { using N = Syntax::String<c_num, nt_number2>; };

template <char c> struct Syntax::Move<nt_part,c> { using N = Syntax::String<nt_content, nt_modifier>; };
MOVE(nt_part,')') { using N = RejectInput; };
MOVE(nt_part,'*') { using N = RejectInput; };
MOVE(nt_part,'+') { using N = RejectInput; };
MOVE(nt_part,',') { using N = RejectInput; };
MOVE(nt_part,'?') { using N = RejectInput; };
MOVE(nt_part,']') { using N = RejectInput; };
MOVE(nt_part,'{') { using N = RejectInput; };
MOVE(nt_part,'|') { using N = RejectInput; };
MOVE(nt_part,'}') { using N = RejectInput; };

template <char c> struct Syntax::Move<nt_pset,c> { using N = Syntax::String<c_alpha, nt_pset>; };
MOVE(nt_pset,'$') { using N = RejectInput; };
MOVE(nt_pset,'(') { using N = RejectInput; };
MOVE(nt_pset,')') { using N = RejectInput; };
MOVE(nt_pset,'*') { using N = RejectInput; };
MOVE(nt_pset,'+') { using N = RejectInput; };
MOVE(nt_pset,',') { using N = RejectInput; };
MOVE(nt_pset,'?') { using N = RejectInput; };
MOVE(nt_pset,'[') { using N = RejectInput; };
MOVE(nt_pset,']') { using N = Epsilon; };
MOVE(nt_pset,'^') { using N = RejectInput; };
MOVE(nt_pset,'.') { using N = RejectInput; };
MOVE(nt_pset,'{') { using N = RejectInput; };
MOVE(nt_pset,'|') { using N = RejectInput; };
MOVE(nt_pset,'}') { using N = RejectInput; };

template <char c> struct Syntax::Move<nt_selection2,c> { using N = Epsilon; };
template <> struct Syntax::Move<nt_selection2> { using N = Epsilon; };
MOVE(nt_selection2,'*') { using N = RejectInput; };
MOVE(nt_selection2,'+') { using N = RejectInput; };
MOVE(nt_selection2,',') { using N = RejectInput; };
MOVE(nt_selection2,'?') { using N = RejectInput; };
MOVE(nt_selection2,']') { using N = RejectInput; };
MOVE(nt_selection2,'{') { using N = RejectInput; };
MOVE(nt_selection2,'|') { using N = Syntax::String<c_pipe, nt_part, nt_selection2>; };
MOVE(nt_selection2,'}') { using N = RejectInput; };

template <char c> struct Syntax::Move<nt_selection,c> { using N = Syntax::String<nt_part, nt_selection2>; };
MOVE(nt_selection,')') { using N = RejectInput; };
MOVE(nt_selection,'*') { using N = RejectInput; };
MOVE(nt_selection,'+') { using N = RejectInput; };
MOVE(nt_selection,',') { using N = RejectInput; };
MOVE(nt_selection,'?') { using N = RejectInput; };
MOVE(nt_selection,']') { using N = RejectInput; };
MOVE(nt_selection,'{') { using N = RejectInput; };
MOVE(nt_selection,'|') { using N = RejectInput; };
MOVE(nt_selection,'}') { using N = RejectInput; };
MOVE(nt_set,'[') { using N = Syntax::String<c_sopen, nt_B>; };

MOVE(c_dolar,'$') { using N = ReadChar; };
MOVE(c_open,'(') { using N = ReadChar; };
MOVE(c_close,')') { using N = ReadChar; };
MOVE(c_star,'*') { using N = ReadChar; };
MOVE(c_plus,'+') { using N = ReadChar; };
MOVE(c_comma,',') { using N = ReadChar; };
MOVE(c_questionmark,'?') { using N = ReadChar; };
MOVE(c_sopen,'[') { using N = ReadChar; };
MOVE(c_sclose,']') { using N = ReadChar; };
MOVE(c_caret,'^') { using N = ReadChar; };
MOVE(c_dot,'.') { using N = ReadChar; };
MOVE(c_copen,'{') { using N = ReadChar; };
MOVE(c_pipe,'|') { using N = ReadChar; };
MOVE(c_cclose,'}') { using N = ReadChar; };

MOVE(c_num,'0') { using N = ReadChar; };
MOVE(c_num,'1') { using N = ReadChar; };
MOVE(c_num,'2') { using N = ReadChar; };
MOVE(c_num,'3') { using N = ReadChar; };
MOVE(c_num,'4') { using N = ReadChar; };
MOVE(c_num,'5') { using N = ReadChar; };
MOVE(c_num,'6') { using N = ReadChar; };
MOVE(c_num,'7') { using N = ReadChar; };
MOVE(c_num,'8') { using N = ReadChar; };
MOVE(c_num,'9') { using N = ReadChar; };

MOVE(c_alpha,'$') { using N = RejectInput; };
MOVE(c_alpha,'(') { using N = RejectInput; };
MOVE(c_alpha,')') { using N = RejectInput; };
MOVE(c_alpha,'*') { using N = RejectInput; };
MOVE(c_alpha,'+') { using N = RejectInput; };
MOVE(c_alpha,',') { using N = RejectInput; };
MOVE(c_alpha,'?') { using N = RejectInput; };
MOVE(c_alpha,'[') { using N = RejectInput; };
MOVE(c_alpha,']') { using N = RejectInput; };
MOVE(c_alpha,'^') { using N = RejectInput; };
MOVE(c_alpha,'.') { using N = RejectInput; };
MOVE(c_alpha,'{') { using N = RejectInput; };
MOVE(c_alpha,'|') { using N = RejectInput; };
MOVE(c_alpha,'}') { using N = RejectInput; };
template <char c> struct Syntax::Move<c_alpha,c> { using N = ReadChar; };


template <typename Syntax> class RegularExpression {
public:
	bool operator()(const char * input) {
		return Syntax::template parse();
	}
	bool checkSyntax() const {
		return Syntax::template check();
	}
};

template<typename CharT, CharT ... string> constexpr auto operator""_regexp() {
	using RegexpDef = Syntax::Parser<Regexp::nt_S, string...>;
	static_assert(RegexpDef::check(), "bad syntax of regular expression");
	return RegularExpression<RegexpDef>{};
}

int main() {
	if ("^(abc)+$"_regexp.checkSyntax()) {
		puts("correct");
	} else {
		puts("not correct");
	}
}
