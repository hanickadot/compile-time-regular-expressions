#include "common.hpp"

int main (int argc, char ** argv)
{
	int errornumber = 0;
	size_t erroroffset = 0;
	const auto * pattern = reinterpret_cast<const unsigned char *>(PATTERN);
	pcre2_code * re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
	
	if (!re) {
		std::cerr << "compilation failed\n";
		return 1;
	}
	
	pcre2_match_context * mcontext = pcre2_match_context_create(NULL);
	
	pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
	pcre2_jit_stack * jit_stack = pcre2_jit_stack_create(32*1024, 512*1024, NULL);
	pcre2_jit_stack_assign(mcontext, NULL, jit_stack);
	
	auto match_data = pcre2_match_data_create_from_pattern(re, NULL);
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return pcre2_match(re, reinterpret_cast<const unsigned char *>(line.data()), line.length(), 0, 0, match_data, mcontext) >= 0;
	});
	
	pcre2_code_free(re);
}

