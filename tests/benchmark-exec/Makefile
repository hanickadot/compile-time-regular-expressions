.PHONY: default all run clean header

PATTERN := ABCD|DEFGH|EFGHI|A{4,}
FILE := input.txt

PATTERN_HASH := $(shell echo "$(PATTERN)" | /sbin/md5)

LIBRARIES := $(sort baseline boost ctre pcre pcre-jit re2 std xpressive srell)

default: all

CXXFLAGS := -std=c++2a -I../../include -Isrell_include
LDFLAGS :=

ctre_CXXFLAGS := 
ctre_LDFLAGS := 
ctre_NAME := CTRE

boost_CXXFLAGS := 
boost_LDFLAGS := -lboost_regex
boost_NAME := boost::regex

pcre_CXXFLAGS :=
pcre_LDFLAGS := -lpcre2-8
pcre_NAME := PCRE2

pcre-jit_CXXFLAGS :=
pcre-jit_LDFLAGS := -lpcre2-8
pcre-jit_NAME := PCRE2 (jit)

re2_CXXFLAGS :=
re2_LDFLAGS := -lre2
re2_NAME := RE2

std_NAME := std::regex

xpressive_NAME := boost::xpressive

srell_NAME := srell

all: $(LIBRARIES)

$(PATTERN_HASH).tmp:
	@rm -f *.tmp
	@touch $@

pattern.hpp: $(PATTERN_HASH).tmp
	@echo "#define PATTERN \"$(PATTERN)\"" > pattern.hpp

$(LIBRARIES:%=%.o): %.o: %.cpp common.hpp $(PATTERN_HASH).tmp pattern.hpp
	$(CXX) -O3 $(CXXFLAGS) ${${@:%.o=%}_CXXFLAGS} -c $< -o $@

$(LIBRARIES): %: %.o common.hpp $(PATTERN_HASH).tmp pattern.hpp
	$(CXX) -O3 $(LDFLAGS) ${${@}_LDFLAGS} $< -o $@

header.csv:
	echo "library;pattern;duration" > header.csv
	echo "library;pattern;duration" > result.csv

$(LIBRARIES:%=%.run): %.run: % header
	./$< $(FILE) benchmark "${${<}_NAME}" 2>/dev/null >>result.csv

run: $(LIBRARIES:%=%.run)
	
node:
	node node-v8.js path=${FILE} pattern="${PATTERN}"

jsc:
	jsc jsc.js -- path=${FILE} pattern="${PATTERN}"

result.csv: header.csv $(LIBRARIES:%=%.run)
	touch result.csv

clean:
	rm -f $(LIBRARIES) *.tmp header.csv result.csv
	