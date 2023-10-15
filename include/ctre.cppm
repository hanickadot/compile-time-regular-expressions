module;

// this will enable in-module mode and include/import std library
#include "ctre/std_module.hpp" 
// CTRE in in-module mode won't include anything
#include "ctre.hpp"

export module ctre;

// this won't export everything in ctre namespace, but only things I take here...
export namespace ctre {
    // so you can pipe a range into a regex wrapper (for split)
    using ctre::operator|;
    
    // modifiers
    using ctre::singleline;
    using ctre::multiline;

    // singleline niebloids
    using ctre::match; // whole input range must match regex
    using ctre::search; // input subrange must match regex
    using ctre::starts_with; // prefix of input must match regex
    
    // singleline range niebloids
    using ctre::search_all; // will look for each occurence of regex
    using ctre::range; // alias to search_all
    using ctre::split; // will split input range by the regex
    using ctre::tokenize; // will split input to pieces matching regex

    // multiline niebloids
    using ctre::multiline_match;
    using ctre::multiline_search;
    using ctre::multiline_starts_with;
    
    // multiline range niebloids
    using ctre::multiline_search_all;
    using ctre::multiline_range;
    using ctre::multiline_split;
    using ctre::multiline_tokenize;
}
