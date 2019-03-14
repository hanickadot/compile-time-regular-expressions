#include "load-file.hpp"
#include <iostream>
#include <chrono>
#include "ctre.hpp"
#include <boost/regex.hpp>
#include <regex>
#include <re2/re2.h>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

#ifndef PATTERN
#define PATTERN "([[:alpha:]]+ing)"
#endif

#define PATTERN_CTRE 

std::pair<std::chrono::nanoseconds, std::chrono::nanoseconds> calculate_diff(const std::vector<std::chrono::nanoseconds> & in) {
	std::chrono::nanoseconds sum = std::accumulate(in.begin(), in.end(), std::chrono::nanoseconds{});
	std::chrono::nanoseconds avg = std::chrono::nanoseconds{sum.count() / in.size()};
	
	int64_t avg_value = avg.count();
	int64_t square_diff_sum = 0;
	
	std::for_each(in.begin(), in.end(), [&](std::chrono::nanoseconds val){
		square_diff_sum += (val.count() - avg_value) * (val.count() - avg_value);
	});
	
	square_diff_sum /= in.size();
	
	int64_t std_diff = sqrt(square_diff_sum);
	
	return {avg, std::chrono::nanoseconds{std_diff}};
}

static constexpr inline auto ctre_pattern = ctll::fixed_string{PATTERN};

int main(int argc, char ** argv) {
	const char * fname = "mtent12.txt";
	std::string_view type = "ctre";
	unsigned repeat = 10;
	
	if (argc >= 4) {
		repeat = std::atoi(argv[3]);
	}
	
	if (argc >= 3) {
		fname = argv[2];
	}
	
	if (argc >= 2) {
		type = std::string_view(argv[1]);
	}
	
	// boost init
	auto boost_init_start = std::chrono::high_resolution_clock::now();
	boost::regex boost_engine = boost::regex(PATTERN);
	auto boost_init_end = std::chrono::high_resolution_clock::now();
	
	auto std_init_start = std::chrono::high_resolution_clock::now();
	std::regex std_engine = std::regex(PATTERN);
	auto std_init_end = std::chrono::high_resolution_clock::now();
	
	auto re2_init_start = std::chrono::high_resolution_clock::now();
	re2::RE2 re2_engine{PATTERN};
	auto re2_init_end = std::chrono::high_resolution_clock::now();
	
	
	auto file = load_file(fname);
	
	using namespace std::string_view_literals;
	
	unsigned matches{0};
	
	auto measure = [](auto cb) {
		auto start = std::chrono::high_resolution_clock::now();
		cb();
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	};
	
	auto ctre_benchmark = [&]{
		
		for (const auto m: ctre::range(file.c_str(), ctre::re<ctre_pattern>())) {
			++matches;
			//std::cout << std::string_view(m) << "\n";
		}
	};
	
	auto boost_benchmark = [&]{
		boost::cregex_iterator m1(file.c_str(), file.c_str() + file.length(), boost_engine), m2;
		for(;m1!=m2;++m1){
			++matches;
		}
	};
	
	auto std_benchmark = [&]{
		std::cregex_iterator m1(file.c_str(), file.c_str() + file.length(), std_engine), m2;
		for(;m1!=m2;++m1){
			++matches;
		}
	};
	
	auto re2_benchmark = [&]{
		re2::StringPiece span{file};
		std::string found;
		while(re2_engine.FindAndConsume(&span, re2_engine, &found)){
			++matches;
			//std::cout << found << "\n";
		}	
	};
	
	std::chrono::nanoseconds duration;
	
	auto start = std::chrono::high_resolution_clock::now();
	if (type == "all"sv) {
		std::vector<std::chrono::nanoseconds> duration_ctre{};
		unsigned matches_ctre{0};
		std::vector<std::chrono::nanoseconds> duration_boost{};
		unsigned matches_boost{0};
		std::vector<std::chrono::nanoseconds> duration_std{};
		unsigned matches_std{0};
		std::vector<std::chrono::nanoseconds> duration_re2{};
		unsigned matches_re2{0};
		
		int count = repeat;
		
		std::cout << "pattern = " << PATTERN << "\n";
		
		for (unsigned int i{0}; i != count; ++i) {
			matches = 0;
			duration_ctre.push_back(measure(ctre_benchmark));
			matches_ctre += matches;
			
			matches = 0;
			duration_boost.push_back(measure(boost_benchmark));
			matches_boost += matches;
			
			matches = 0;
			duration_std.push_back(measure(std_benchmark));
			matches_std += matches;
			
			matches = 0;
			duration_re2.push_back(measure(re2_benchmark));
			matches_re2 += matches;
			
			if (matches_ctre != matches_boost) {
				std::cout << "different output! (ctre <> boost)\n";
				return 1;
			}
			
			if (matches_ctre != matches_std) {
				std::cout << "different output! (ctre <> std)\n";
				return 1;
			}
			
			if (matches_ctre != matches_re2) {
				std::cout << "different output! (ctre <> re2)\n";
				return 1;
			}
			
			std::cerr << "iteration " << (i+1) << " done...\n";
		}
		
		auto print = [](std::string_view name, const auto & vec) {
			auto [avg, stddiff] = calculate_diff(vec);
			std::cout << name << " = " << std::fixed << std::setw( 11 ) << std::setprecision( 3 );
			std::cout << (avg.count() / 1'000'000.0f) << " ms";
			std::cout << " (stdev = " << std::fixed << std::setw( 11 ) << std::setprecision( 3 );
			std::cout << (stddiff.count() / 1'000'000.0f) << " ms)\n";
		};
		
		print("ctre ",duration_ctre);
		print("boost",duration_boost);
		print("std  ",duration_std);
		print("re2  ",duration_re2);
		return 0;
		
	} else if (type == "ctre"sv) {
		duration = measure(ctre_benchmark);
	} else if (type == "boost"sv) {
		duration = measure(boost_benchmark);
	} else if (type == "std"sv) {
		duration = measure(std_benchmark);
	} else if (type == "re2"sv) {
		duration = measure(re2_benchmark);
	} else {
		std::cerr << "unknown regex engine!\n";
		return 1;
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	auto diff = end - start;
	std::cerr << "matches = " << matches << "\n";
	if (type == "ctre"sv) {
		std::cerr << "init = null\n";
	} else {
		double init = 0.0;
		if (type == "boost"sv) {
			init =  std::chrono::duration_cast<std::chrono::nanoseconds>(boost_init_end - boost_init_start).count();
		} else if (type == "std"sv) {
			init =  std::chrono::duration_cast<std::chrono::nanoseconds>(std_init_end - std_init_start).count();
		} else if (type == "re2"sv) {
			init =  std::chrono::duration_cast<std::chrono::nanoseconds>(re2_init_end - re2_init_start).count();
		}
		std::cerr << "init = " << (init / 1'000'000.0f) << " ms\n";
	}
	std::cerr << "measured = " << duration.count() / 1'000'000.0f << " ms\n";
}