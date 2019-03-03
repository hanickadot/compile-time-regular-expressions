#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"
#include <re2/re2.h>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <chrono>
#include "pattern.hpp"
#include <srell.hpp>

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

enum class benchmark_mode {
	normal,
	off,
	benchmark
};

struct CannotLoadFile {};

static std::vector<char> loadBlob(const char * fname) {
	FILE * fd = fopen(fname, "rb");
	if (!fd) throw CannotLoadFile();
	fseek(fd, 0, SEEK_END);
	size_t size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	std::vector<char> out;
	out.resize(size);
	fread(out.data(), size, 1, fd);
	fclose(fd);
	return out;
}

[[maybe_unused]] static std::pair<std::vector<std::string_view>, std::vector<char>> loadFile(const char * fname) {
	std::vector<std::string_view> out;
	std::vector<char> blob = loadBlob(fname);
	
	auto start = blob.begin();
	auto it = blob.begin();
	const auto end = blob.end();
	
	bool previous_was_r = false;
	
	while (it != end) {
		if (*it == '\r') {
			*it = '\0';
			previous_was_r = true;
		} else if (*it == '\n') {
			*it = '\0';
			out.push_back(std::string_view(std::addressof(*start), std::distance(start, it - (previous_was_r ? 1 : 0))));
			start = std::next(it);
			previous_was_r = false;
		} else {
			previous_was_r = false;
		}
		it = std::next(it);
	}
	
	return {std::move(out), std::move(blob)};
}

template <typename CB> int benchmark_over_file(const char * fname, benchmark_mode mode, CB && cb, std::string_view app) {
	std::ios_base::sync_with_stdio(false);
	try {
		auto [lines, blob] = loadFile(fname);
		
		uint64_t count{0};
		
		std::cerr << "Start...\n" << std::flush;
		
		auto start = std::chrono::steady_clock::now();
		
		for (auto line: lines) {
			if (cb(line)) {
				if (mode == benchmark_mode::normal) std::cout << line << '\n';
				count++;
			}
		}
		
		auto end = std::chrono::steady_clock::now();
		
		if (mode == benchmark_mode::benchmark) {
			if (app.length()) {
				std::cout << '"' << app << "\";" << '"' << PATTERN << "\";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
			}
		}
		std::cerr << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
		std::cerr << "Duration: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns\n";
		
		std::cerr << "Matches: " << count << "\n";
		std::cerr << "Lines: " << lines.size() << "\n";
		std::cerr << "Percentage: " << double(count) * 100 / lines.size() << "\n";
		return 0;
		
	} catch (CannotLoadFile) {
		std::cerr << "Can't load file.\n";
		return 1;
	}
}

template <typename CB> int benchmark(int argc, char ** argv, CB && cb) {
	if (argc == 0) {
		std::cerr << "There are no arguments!\n";
		return 3;
	} else if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " filaname [off]\n";
		return 2;
	}
	bool off = false;
	benchmark_mode mode{benchmark_mode::normal};
	
	std::string_view app = argv[0];
	
	if (argc >= 3) {
		if (std::string_view(argv[2]) == "off") {
			mode = benchmark_mode::off;
		} else if (std::string_view(argv[2]) == "benchmark") {
			mode = benchmark_mode::benchmark;
		}
	}
	
	if (argc >= 4) {
		app = std::string_view(argv[3]);
	}
	
	return benchmark_over_file(argv[1], mode, std::forward<CB>(cb), app);
}

