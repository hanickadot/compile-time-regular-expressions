namespace Grammar {
using namespace Static;
// nonterminals
// ============
SYMBOL(NT_S);
SYMBOL(NT_A);
SYMBOL(NT_alt2);
SYMBOL(NT_alt);
SYMBOL(NT_altB);
SYMBOL(NT_atom);
SYMBOL(NT_atom_repeat);
SYMBOL(NT_B);
SYMBOL(NT_backslash);
SYMBOL(NT_backslash_char);
SYMBOL(NT_block);
SYMBOL(NT_C);
SYMBOL(NT_char);
SYMBOL(NT_class);
SYMBOL(NT_D);
SYMBOL(NT_E);
SYMBOL(NT_F);
SYMBOL(NT_G);
SYMBOL(NT_H);
SYMBOL(NT_mod);
SYMBOL(NT_nonchar);
SYMBOL(NT_number2);
SYMBOL(NT_number);
SYMBOL(NT_range);
SYMBOL(NT_repeat);
SYMBOL(NT_set2);
SYMBOL(NT_set);
SYMBOL(NT_setitem);
SYMBOL(NT_special);
SYMBOL(NT_string2);
SYMBOL(NT_string);
// terminals
// =========
SYMBOL(Term_Pipe);
SYMBOL(Term_pipe);
SYMBOL(Term_Plus);
SYMBOL(Term_plus);
SYMBOL(Term_Star);
SYMBOL(Term_star);
SYMBOL(Term_QuestionMark);
SYMBOL(Term_questionmark);
SYMBOL(Term_COpen);
SYMBOL(Term_other);
SYMBOL(Term_Comma);
SYMBOL(Term_Minus);
SYMBOL(Term_0);
SYMBOL(Term_1);
SYMBOL(Term_2);
SYMBOL(Term_3);
SYMBOL(Term_4);
SYMBOL(Term_5);
SYMBOL(Term_6);
SYMBOL(Term_7);
SYMBOL(Term_8);
SYMBOL(Term_9);
SYMBOL(Term_Colon);
SYMBOL(Term_SClose);
SYMBOL(Term_a);
SYMBOL(Term_b);
SYMBOL(Term_c);
SYMBOL(Term_d);
SYMBOL(Term_e);
SYMBOL(Term_f);
SYMBOL(Term_x);
SYMBOL(Term_nonspecial);
SYMBOL(Term_Dot);
SYMBOL(Term_dot);
SYMBOL(Term_Backslash);
SYMBOL(Term_Caret);
SYMBOL(Term_caret);
SYMBOL(Term_Dolar);
SYMBOL(Term_dolar);
SYMBOL(Term_Open);
SYMBOL(Term_SOpen);
SYMBOL(Term_num);
SYMBOL(Term_nonspecial2);
SYMBOL(Term_octal);
SYMBOL(Term_sopen);
SYMBOL(Term_copen);
SYMBOL(Term_CClose);
SYMBOL(Term_cclose);
SYMBOL(Term_open);
SYMBOL(Term_Close);
SYMBOL(Term_close);
SYMBOL(Term_backslash);
SYMBOL(Term_colon);
SYMBOL(Term_sclose);
SYMBOL(Term_hexdec);
}
// end of namespace for symbols

// TABLE of LL1
// ============
template <> struct Static::Table<Grammar::NT_S,'$'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'('> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,','> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'-'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'.'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'0'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'1'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'2'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'3'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'4'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'5'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'6'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'7'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'8'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'9'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,':'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'['> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'\\'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,']'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'^'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'a'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'b'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'c'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'d'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'e'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'f'> { using Move = Grammar::NT_alt; }; // last 
template <> struct Static::Table<Grammar::NT_S,'x'> { using Move = Grammar::NT_alt; }; // last 
template <char c> struct Static::Table<Grammar::NT_S,c> { using Move = Grammar::NT_alt; }; // other
template <> struct Static::Table<Grammar::NT_S> { using Move = Static::Epsilon; }; // epsilon
template <> struct Static::Table<Grammar::NT_S,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_S,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_A,'}'> { using Move = Static::String<Static::Builder<repeat_exactly>, Grammar::Term_cclose>; }; // last 
template <> struct Static::Table<Grammar::NT_A,','> { using Move = Static::String<Grammar::Term_Comma, Grammar::NT_H>; }; // last 


template <> struct Static::Table<Grammar::NT_alt2,'|'> { using Move = Static::String<Grammar::Term_pipe, Grammar::NT_alt, Static::Builder<alt>>; }; // last 
template <> struct Static::Table<Grammar::NT_alt2,')'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_alt2> { using Move = Static::Epsilon; }; // epsilon


template <> struct Static::Table<Grammar::NT_alt,'$'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'('> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,','> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'-'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'.'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'0'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'1'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'2'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'3'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'4'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'5'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'6'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'7'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'8'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'9'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,':'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'['> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'\\'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,']'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'^'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'a'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'b'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'c'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'d'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'e'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'f'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_alt,'x'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <char c> struct Static::Table<Grammar::NT_alt,c> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // other
template <> struct Static::Table<Grammar::NT_alt,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_alt,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_altB,'$'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'('> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,','> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'-'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'.'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'0'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'1'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'2'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'3'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'4'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'5'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'6'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'7'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'8'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'9'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,':'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'['> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'\\'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,']'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'^'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'a'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'b'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'c'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'d'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'e'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'f'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,'x'> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // last 
template <> struct Static::Table<Grammar::NT_altB,')'> { using Move = Static::Builder<empty>; }; // last 
template <char c> struct Static::Table<Grammar::NT_altB,c> { using Move = Static::String<Grammar::NT_string, Grammar::NT_alt2>; }; // other
template <> struct Static::Table<Grammar::NT_altB,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_altB,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_altB,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_altB,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_altB,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_altB,'}'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_atom,','> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'-'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'.'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'0'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'1'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'2'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'3'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'4'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'5'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'6'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'7'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'8'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'9'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,':'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'\\'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,']'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'a'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'b'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'c'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'d'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'e'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'f'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'x'> { using Move = Grammar::NT_char; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'$'> { using Move = Grammar::NT_nonchar; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'^'> { using Move = Grammar::NT_nonchar; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'('> { using Move = Grammar::NT_block; }; // last 
template <> struct Static::Table<Grammar::NT_atom,'['> { using Move = Grammar::NT_class; }; // last 
template <char c> struct Static::Table<Grammar::NT_atom,c> { using Move = Grammar::NT_char; }; // other
template <> struct Static::Table<Grammar::NT_atom,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_atom_repeat,'$'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'('> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,','> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'-'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'.'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'0'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'1'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'2'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'3'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'4'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'5'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'6'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'7'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'8'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'9'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,':'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'['> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'\\'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,']'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'^'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'a'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'b'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'c'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'d'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'e'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'f'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <> struct Static::Table<Grammar::NT_atom_repeat,'x'> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // last 
template <char c> struct Static::Table<Grammar::NT_atom_repeat,c> { using Move = Static::String<Grammar::NT_atom, Grammar::NT_repeat>; }; // other
template <> struct Static::Table<Grammar::NT_atom_repeat,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_atom_repeat,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_B,'$'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'('> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,')'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'*'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'+'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'.'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'0'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'?'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'['> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'\\'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'^'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'a'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'x'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'{'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'|'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'}'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_B,'d'> { using Move = Grammar::NT_backslash_char; }; // last 


template <> struct Static::Table<Grammar::NT_backslash,'$'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'('> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,')'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'*'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'+'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'.'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'?'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'['> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'\\'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'^'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'{'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'|'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'}'> { using Move = Static::String<Static::Builder<addchar>, Grammar::NT_special>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'a'> { using Move = Static::String<Grammar::Term_a, Static::Builder<backa>>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'0'> { using Move = Static::String<Grammar::Term_0, Static::Builder<odigit>, Grammar::Term_octal, Static::Builder<odigit>, Grammar::Term_octal, Static::Builder<odigit>, Grammar::Term_octal, Static::Builder<octal3>>; }; // last 
template <> struct Static::Table<Grammar::NT_backslash,'x'> { using Move = Static::String<Grammar::Term_x, Grammar::NT_G>; }; // last 


template <char... c> struct Static::Table<Grammar::NT_backslash_char,c...> { using Move = Static::String<Grammar::Term_d, Static::Builder<class_digit>>; }; // only one rule 


template <char... c> struct Static::Table<Grammar::NT_block,c...> { using Move = Static::String<Grammar::Term_Open, Grammar::NT_C>; }; // only one rule 


template <> struct Static::Table<Grammar::NT_C,'$'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'('> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,')'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,','> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'-'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'.'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'0'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'1'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'2'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'3'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'4'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'5'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'6'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'7'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'8'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'9'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,':'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'['> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'\\'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,']'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'^'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'a'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'b'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'c'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'d'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'e'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'f'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'x'> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <> struct Static::Table<Grammar::NT_C,'?'> { using Move = Static::String<Grammar::Term_questionmark, Grammar::Term_colon, Grammar::NT_altB, Grammar::Term_close>; }; // last 
template <char c> struct Static::Table<Grammar::NT_C,c> { using Move = Static::String<Grammar::NT_altB, Grammar::Term_close>; }; // other
template <> struct Static::Table<Grammar::NT_C,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_C,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_C,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_C,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_C,'}'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_char,','> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'-'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'0'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'1'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'2'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'3'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'4'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'5'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'6'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'7'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'8'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'9'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,':'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,']'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'a'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'b'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'c'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'d'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'e'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'f'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'x'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'.'> { using Move = Static::String<Grammar::Term_dot, Static::Builder<any>>; }; // last 
template <> struct Static::Table<Grammar::NT_char,'\\'> { using Move = Static::String<Grammar::Term_Backslash, Grammar::NT_B>; }; // last 
template <char c> struct Static::Table<Grammar::NT_char,c> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_other>; }; // other
template <> struct Static::Table<Grammar::NT_char,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_char,')'> { using Move = Static::RejectInput; }; // sigma 


template <char... c> struct Static::Table<Grammar::NT_class,c...> { using Move = Static::String<Grammar::Term_SOpen, Grammar::NT_D>; }; // only one rule 


template <> struct Static::Table<Grammar::NT_D,'.'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'0'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'1'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'2'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'3'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'4'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'5'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'6'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'7'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'8'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'9'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,':'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'\\'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'a'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'b'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'c'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'d'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'e'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'f'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'x'> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <> struct Static::Table<Grammar::NT_D,'^'> { using Move = Static::String<Grammar::Term_caret, Static::Builder<set_neg_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // last 
template <char c> struct Static::Table<Grammar::NT_D,c> { using Move = Static::String<Static::Builder<set_start>, Grammar::NT_set, Grammar::Term_sclose, Static::Builder<set_finish>>; }; // other
template <> struct Static::Table<Grammar::NT_D,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'-'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,']'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_D,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_E,'$'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'('> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,')'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'*'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'+'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'.'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'0'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'?'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'['> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'\\'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'^'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'a'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'x'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'{'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'|'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'}'> { using Move = Grammar::NT_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_E,'b'> { using Move = Static::String<Grammar::Term_b, Static::Builder<backb>>; }; // last 


template <> struct Static::Table<Grammar::NT_F,'0'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'1'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'2'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'3'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'4'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'5'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'6'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'7'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'8'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'9'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,':'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'a'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'b'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'c'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'d'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'e'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'f'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <> struct Static::Table<Grammar::NT_F,'x'> { using Move = Static::String<Static::Builder<range>, Grammar::Term_nonspecial2>; }; // last 
template <char c> struct Static::Table<Grammar::NT_F,c> { using Move = Static::String<Static::Builder<range>, Grammar::Term_other>; }; // other
template <> struct Static::Table<Grammar::NT_F,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'-'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,']'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'.'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'\\'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_F,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_G,'0'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'1'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'2'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'3'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'4'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'5'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'6'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'7'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'8'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'9'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'a'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'b'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'c'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'d'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'e'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'f'> { using Move = Static::String<Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hexdec2>>; }; // last 
template <> struct Static::Table<Grammar::NT_G,'{'> { using Move = Static::String<Grammar::Term_copen, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Static::Builder<hdigit>, Grammar::Term_hexdec, Grammar::Term_cclose, Static::Builder<hexdec4>>; }; // last 


template <> struct Static::Table<Grammar::NT_H,'}'> { using Move = Static::String<Static::Builder<repeat_at_least>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'0'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'1'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'2'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'3'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'4'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'5'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'6'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'7'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'8'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_H,'9'> { using Move = Static::String<Grammar::NT_number, Static::Builder<repeat_ab>, Grammar::Term_cclose, Grammar::NT_mod>; }; // last 


template <> struct Static::Table<Grammar::NT_mod,'$'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'('> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,')'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,','> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'-'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'.'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'0'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'1'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'2'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'3'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'4'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'5'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'6'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'7'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'8'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'9'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,':'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'['> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'\\'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,']'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'^'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'a'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'b'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'c'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'d'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'e'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'f'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'x'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'|'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'?'> { using Move = Static::String<Grammar::Term_questionmark, Static::Builder<nongreedy>>; }; // last 
template <> struct Static::Table<Grammar::NT_mod,'+'> { using Move = Static::String<Grammar::Term_plus, Static::Builder<possessive>>; }; // last 
template <char c> struct Static::Table<Grammar::NT_mod,c> { using Move = Static::Epsilon; }; // other
template <> struct Static::Table<Grammar::NT_mod> { using Move = Static::Epsilon; }; // epsilon
template <> struct Static::Table<Grammar::NT_mod,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_mod,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_mod,'}'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_nonchar,'^'> { using Move = Static::String<Grammar::Term_caret, Static::Builder<insert_begin>>; }; // last 
template <> struct Static::Table<Grammar::NT_nonchar,'$'> { using Move = Static::String<Grammar::Term_dolar, Static::Builder<insert_end>>; }; // last 


template <> struct Static::Table<Grammar::NT_number2,','> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'}'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'0'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'1'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'2'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'3'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'4'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'5'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'6'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'7'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'8'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 
template <> struct Static::Table<Grammar::NT_number2,'9'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2>; }; // last 


template <> struct Static::Table<Grammar::NT_number,'0'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'1'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'2'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'3'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'4'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'5'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'6'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'7'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'8'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 
template <> struct Static::Table<Grammar::NT_number,'9'> { using Move = Static::String<Static::Builder<digit>, Grammar::Term_num, Grammar::NT_number2, Static::Builder<number>>; }; // last 


template <> struct Static::Table<Grammar::NT_range,'.'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'0'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'1'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'2'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'3'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'4'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'5'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'6'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'7'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'8'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'9'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,':'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'\\'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,']'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'a'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'b'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'c'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'d'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'e'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'f'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'x'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_range,'-'> { using Move = Static::String<Grammar::Term_Minus, Grammar::NT_F>; }; // last 
template <char c> struct Static::Table<Grammar::NT_range,c> { using Move = Static::Epsilon; }; // other
template <> struct Static::Table<Grammar::NT_range> { using Move = Static::Epsilon; }; // epsilon
template <> struct Static::Table<Grammar::NT_range,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_range,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_repeat,'$'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'('> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,')'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,','> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'-'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'.'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'0'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'1'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'2'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'3'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'4'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'5'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'6'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'7'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'8'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'9'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,':'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'['> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'\\'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,']'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'^'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'a'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'b'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'c'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'d'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'e'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'f'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'x'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'|'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'+'> { using Move = Static::String<Grammar::Term_plus, Static::Builder<plus>, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'*'> { using Move = Static::String<Grammar::Term_star, Static::Builder<star>, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'?'> { using Move = Static::String<Grammar::Term_questionmark, Static::Builder<optional>, Grammar::NT_mod>; }; // last 
template <> struct Static::Table<Grammar::NT_repeat,'{'> { using Move = Static::String<Grammar::Term_COpen, Grammar::NT_number, Grammar::NT_A>; }; // last 
template <char c> struct Static::Table<Grammar::NT_repeat,c> { using Move = Static::Epsilon; }; // other
template <> struct Static::Table<Grammar::NT_repeat> { using Move = Static::Epsilon; }; // epsilon
template <> struct Static::Table<Grammar::NT_repeat,'}'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_set2,'.'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'0'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'1'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'2'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'3'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'4'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'5'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'6'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'7'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'8'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'9'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,':'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'\\'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'a'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'b'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'c'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'d'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'e'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'f'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,'x'> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set2,']'> { using Move = Static::Epsilon; }; // last 
template <char c> struct Static::Table<Grammar::NT_set2,c> { using Move = Static::String<Grammar::NT_setitem, Static::Builder<set_combine>, Grammar::NT_set2>; }; // other
template <> struct Static::Table<Grammar::NT_set2,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'-'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set2,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_set,'.'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'0'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'1'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'2'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'3'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'4'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'5'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'6'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'7'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'8'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'9'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,':'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'\\'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'a'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'b'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'c'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'d'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'e'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'f'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <> struct Static::Table<Grammar::NT_set,'x'> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // last 
template <char c> struct Static::Table<Grammar::NT_set,c> { using Move = Static::String<Grammar::NT_setitem, Grammar::NT_set2>; }; // other
template <> struct Static::Table<Grammar::NT_set,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'-'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,']'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_set,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_setitem,'0'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'1'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'2'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'3'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'4'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'5'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'6'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'7'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'8'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'9'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,':'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'a'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'b'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'c'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'d'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'e'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'f'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'x'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_nonspecial2, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'.'> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_dot, Grammar::NT_range>; }; // last 
template <> struct Static::Table<Grammar::NT_setitem,'\\'> { using Move = Static::String<Grammar::Term_Backslash, Grammar::NT_E>; }; // last 
template <char c> struct Static::Table<Grammar::NT_setitem,c> { using Move = Static::String<Static::Builder<addchar>, Grammar::Term_other, Grammar::NT_range>; }; // other
template <> struct Static::Table<Grammar::NT_setitem,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,','> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'-'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,']'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'^'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'$'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'('> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'['> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_setitem,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_special,'.'> { using Move = Grammar::Term_dot; }; // last 
template <> struct Static::Table<Grammar::NT_special,'['> { using Move = Grammar::Term_sopen; }; // last 
template <> struct Static::Table<Grammar::NT_special,'{'> { using Move = Grammar::Term_copen; }; // last 
template <> struct Static::Table<Grammar::NT_special,'}'> { using Move = Grammar::Term_cclose; }; // last 
template <> struct Static::Table<Grammar::NT_special,'('> { using Move = Grammar::Term_open; }; // last 
template <> struct Static::Table<Grammar::NT_special,')'> { using Move = Grammar::Term_close; }; // last 
template <> struct Static::Table<Grammar::NT_special,'\\'> { using Move = Grammar::Term_backslash; }; // last 
template <> struct Static::Table<Grammar::NT_special,'*'> { using Move = Grammar::Term_star; }; // last 
template <> struct Static::Table<Grammar::NT_special,'+'> { using Move = Grammar::Term_plus; }; // last 
template <> struct Static::Table<Grammar::NT_special,'?'> { using Move = Grammar::Term_questionmark; }; // last 
template <> struct Static::Table<Grammar::NT_special,'|'> { using Move = Grammar::Term_pipe; }; // last 
template <> struct Static::Table<Grammar::NT_special,'^'> { using Move = Grammar::Term_caret; }; // last 
template <> struct Static::Table<Grammar::NT_special,'$'> { using Move = Grammar::Term_dolar; }; // last 


template <> struct Static::Table<Grammar::NT_string2,'$'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'('> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,','> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'-'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'.'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'0'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'1'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'2'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'3'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'4'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'5'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'6'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'7'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'8'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'9'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,':'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'['> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'\\'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,']'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'^'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'a'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'b'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'c'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'d'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'e'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'f'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'x'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // last 
template <> struct Static::Table<Grammar::NT_string2,')'> { using Move = Static::Epsilon; }; // last 
template <> struct Static::Table<Grammar::NT_string2,'|'> { using Move = Static::Epsilon; }; // last 
template <char c> struct Static::Table<Grammar::NT_string2,c> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2, Static::Builder<create_sequence>>; }; // other
template <> struct Static::Table<Grammar::NT_string2> { using Move = Static::Epsilon; }; // epsilon
template <> struct Static::Table<Grammar::NT_string2,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string2,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string2,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string2,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string2,'}'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::NT_string,'$'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'('> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,','> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'-'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'.'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'0'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'1'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'2'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'3'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'4'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'5'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'6'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'7'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'8'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'9'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,':'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'['> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'\\'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,']'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'^'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'a'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'b'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'c'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'d'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'e'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'f'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <> struct Static::Table<Grammar::NT_string,'x'> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // last 
template <char c> struct Static::Table<Grammar::NT_string,c> { using Move = Static::String<Grammar::NT_atom_repeat, Grammar::NT_string2>; }; // other
template <> struct Static::Table<Grammar::NT_string,'|'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,'+'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,'*'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,'?'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,'{'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,'}'> { using Move = Static::RejectInput; }; // sigma 
template <> struct Static::Table<Grammar::NT_string,')'> { using Move = Static::RejectInput; }; // sigma 


template <> struct Static::Table<Grammar::Term_Pipe,'|'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_pipe,'|'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Plus,'+'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_plus,'+'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Star,'*'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_star,'*'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_QuestionMark,'?'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_questionmark,'?'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_COpen,'{'> { using Move = Static::ReadChar; }; 
template <char c> struct Static::Table<Grammar::Term_other,c> { using Move = Static::ReadChar; }; // sigma
template <> struct Static::Table<Grammar::Term_other,'|'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'+'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'*'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'?'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'{'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,','> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'-'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'0'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'1'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'2'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'3'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'4'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'5'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'6'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'7'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'8'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'9'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,':'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,']'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'a'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'b'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'c'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'d'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'e'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'f'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'x'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'.'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'\\'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'^'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'$'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'('> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'['> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,'}'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_other,')'> { using Move = Static::RejectInput; }; // negative for other
template <> struct Static::Table<Grammar::Term_Comma,','> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_Minus,'-'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_0,'0'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_1,'1'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_2,'2'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_3,'3'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_4,'4'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_5,'5'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_6,'6'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_7,'7'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_8,'8'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_9,'9'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_Colon,':'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_SClose,']'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_a,'a'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_b,'b'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_c,'c'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_d,'d'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_e,'e'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_f,'f'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_x,'x'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_nonspecial,','> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'-'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'0'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'1'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'2'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'3'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'4'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'5'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'6'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'7'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'8'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'9'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,':'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,']'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'a'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'b'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'c'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'d'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'e'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'f'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial,'x'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Dot,'.'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_dot,'.'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Backslash,'\\'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_Caret,'^'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_caret,'^'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Dolar,'$'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_dolar,'$'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Open,'('> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_SOpen,'['> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_num,'0'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'1'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'2'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'3'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'4'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'5'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'6'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'7'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'8'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_num,'9'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'0'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'1'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'2'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'3'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'4'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'5'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'6'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'7'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'8'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'9'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,':'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'a'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'b'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'c'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'d'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'e'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'f'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_nonspecial2,'x'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'0'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'1'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'2'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'3'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'4'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'5'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'6'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_octal,'7'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_sopen,'['> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_copen,'{'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_CClose,'}'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_cclose,'}'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_open,'('> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_Close,')'> { using Move = Static::ReadChar; }; 
template <> struct Static::Table<Grammar::Term_close,')'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_backslash,'\\'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_colon,':'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_sclose,']'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'0'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'1'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'2'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'3'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'4'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'5'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'6'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'7'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'8'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'9'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'a'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'b'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'c'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'d'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'e'> { using Move = Static::ReadChar; }; // set
template <> struct Static::Table<Grammar::Term_hexdec,'f'> { using Move = Static::ReadChar; }; // set