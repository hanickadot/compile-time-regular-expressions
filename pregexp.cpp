#include "pregexp.hpp"

int main() {
	//using namespace RegExp;
	//"^a$"_pre.muhaha();
	checkType< sre::RegExp<sre::Begin,sre::Char<'a'>,sre::End> >("^a$"_pre);
	checkType< sre::RegExp<sre::Begin,sre::Char<'a'>,sre::Char<'b'>,sre::Char<'c'>,sre::End> >("^abc$"_pre);
	checkType< sre::RegExp<sre::Optional<sre::Char<'a'>>> >("a?"_pre);
	checkType< sre::RegExp<sre::Plus<sre::Char<'a'>>> >("a+"_pre);
	checkType< sre::RegExp<sre::Star<sre::Char<'a'>>> >("a*"_pre);
	checkType< sre::RegExp<sre::Star<sre::Char<'a'>, sre::Char<'b'>>> >("(?:ab)*"_pre);
	checkType< sre::RegExp<sre::Star<sre::Char<'a'>, sre::Char<'b'>>> >("(ab)*"_pre);
	checkType< sre::RegExp<sre::Begin,sre::Star<sre::Char<'a'>, sre::Char<'b'>>, sre::End> >("^(ab)*$"_pre);
	checkType< sre::RegExp<sre::RepeatHelper<2,2,true,sre::Char<'a'>, sre::Char<'b'>>> >("(ab){2}"_pre);
	checkType< sre::RegExp<sre::Plus<sre::Char<'a'>, sre::Char<'b'>>> >("(ab){1,}"_pre);
	checkType< sre::RegExp<sre::Star<sre::Char<'a'>, sre::Char<'b'>>> >("(ab){0,}"_pre);
	checkType< sre::RegExp<sre::RepeatHelper<1,2,true,sre::Char<'a'>>> >("a{1,2}"_pre);
	checkType< sre::RegExp<sre::RepeatHelper<0,0,false,sre::Char<'a'>, sre::Char<'b'>>> >("(ab)*?"_pre);
	checkType< sre::RegExp<sre::RepeatHelper<1,0,false,sre::Char<'a'>>> >("a+?"_pre);
	checkType< sre::RegExp<sre::Select<sre::Char<'a'>,sre::Char<'b'>>> >("a|b"_pre);
	checkType< sre::RegExp<sre::Select<sre::Char<'a'>,sre::Char<'b'>,sre::Char<'c'>>> >("a|b|c"_pre);
	
	checkType< sre::RegExp<sre::Anything> >("."_pre);
	checkType< sre::RegExp<sre::Begin,sre::Char<'x'>,sre::Plus<sre::Anything>,sre::End> >("^x.+$"_pre);
	checkType< sre::RegExp<sre::Select<sre::Empty,sre::Char<'a'>>> >("()|a"_pre);
	checkType< sre::RegExp<sre::Select<sre::Char<'a'>,sre::Empty>> >("a|()"_pre);
	checkType< sre::RegExp<sre::Select<sre::Char<'a'>,sre::Empty>> >("a|((()))"_pre);
	
	checkType< sre::RegExp<sre::Number> >("\\d"_pre);
	//checkType< sre::RegExp<sre::Range<'a','x'>> >("[a-x]"_pre);
	checkType< sre::RegExp<sre::Char<'a','f'>> >("[af]"_pre);
	checkType< sre::RegExp<sre::Range<'a','f'>> >("[a-f]"_pre);
	checkType< sre::RegExp<sre::Range<'a','f','0','5'>> >("[a-f0-5]"_pre);
	checkType< sre::RegExp<sre::NegativeRange<'a','f','0','5'>> >("[^a-f0-5]"_pre);
	
	assert("^x.+$"_pre.match("xaaa") == true);
	assert("^x.+$"_pre.match("xa") == true);
	assert("^x.+$"_pre.match("yaaaaaa") == false);
	assert("^x.+$"_pre.match("x") == false);
	
	
	assert("^(a|b|c)$"_pre.match("a") == true);
	assert("^(a|b|c)$"_pre.match("b") == true);
	assert("^(a|b|c)$"_pre.match("c") == true);
	assert("^(a|b|c)$"_pre.match("d") == false);
	
	assert("^(ab)*$"_pre.match("") == true);
	assert("^(ab)*$"_pre.match("ababab") == true);
	assert("^(ab)*$"_pre.match("ababa") == false);

	
}
