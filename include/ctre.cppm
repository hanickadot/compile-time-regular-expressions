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
    using ctre::match;
    using ctre::search;
    using ctre::starts_with;
    
    // singleline range niebloids
    using ctre::search_all;
    using ctre::range;
    using ctre::split;
    using ctre::tokenize;
    using ctre::iterator;
    using ctre::sentinel;
    
    // multiline niebloids
    using ctre::multiline_match;
    using ctre::multiline_search;
    using ctre::multiline_starts_with;
    
    // multiline range niebloids
    using ctre::multiline_search_all;
    using ctre::multiline_range;
    using ctre::multiline_split;
    using ctre::multiline_tokenize;
    using ctre::multiline_iterator;
    using ctre::multiline_sentinel;
}
