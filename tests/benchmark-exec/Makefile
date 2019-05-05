.PHONY: default all run clean header

PATTERN := ABCD|DEFGH|EFGHI|A{4,}
FILE := input.txt

PATTERN_HASH := $(shell echo "$(PATTERN)" | /sbin/md5)

LIBRARIES := baseline ctre ctfa ctfa-data srell pcre pcre-jit re2 boost xpressive xpressive-ct std

#LIBRARIES_clang = re2 boost xpressive xpressive-ct

default: all
	
compiler := $(shell $(CXX) -v 2>&1 | grep -E "^clang|^gcc" | head -n1 | sed -E "s/([a-z]+).*/\\1/")

ifeq ($(compiler),clang)
CXXFLAGS_ADDITIONAL := -fconstexpr-steps=100000000
ifeq ($(shell uname),Darwin)
CXXFLAGS_ADDITIONAL += -isysroot  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk
endif
endif

OPTLEVEL := -Os

CXXFLAGS := $(CXXFLAGS_ADDITIONAL) -std=c++2a -I../../include -Isrell_include -I/usr/local/Cellar/pcre2/10.33/include -I/usr/local/Cellar/boost/1.69.0_2/include -I/usr/local/Cellar/re2/20180801/include
LDFLAGS :=

ctre_CXXFLAGS := 
ctre_LDFLAGS := 
ctre_NAME := CTRE

baseline_NAME := baseline

ctfa_NAME := CTFA
ctfa-data_NAME := CTFA (data-driven)

boost_CXXFLAGS := 
boost_LDFLAGS := -lboost_regex
boost_NAME := boost::regex

pcre_CXXFLAGS := 
pcre_LDFLAGS := -L/usr/local/Cellar/pcre2/10.33/lib -lpcre2-8
pcre_NAME := PCRE2

pcre-jit_CXXFLAGS :=
pcre-jit_LDFLAGS := -L/usr/local/Cellar/pcre2/10.33/lib -lpcre2-8
pcre-jit_NAME := PCRE2 (jit)

re2_CXXFLAGS :=
re2_LDFLAGS := -lre2
re2_NAME := RE2

std_NAME := std::regex

xpressive_NAME := boost::xpressive
xpressive-ct_NAME := boost::xpressive (static)

srell_NAME := srell

all: $(LIBRARIES) $(LIBRARIES_$(compiler))

$(PATTERN_HASH).tmp:
	@rm -f *.tmp
	@touch $@

pattern.hpp: $(PATTERN_HASH).tmp
	@echo "#define PATTERN \"$(PATTERN)\"" > pattern.hpp

$(LIBRARIES:%=%.o) $(LIBRARIES_$(compiler):%=%.o): %.o: %.cpp common.hpp $(PATTERN_HASH).tmp pattern.hpp
	$(CXX) $(OPTLEVEL) $(CXXFLAGS) ${${@:%.o=%}_CXXFLAGS} -c $< -o $@

$(LIBRARIES) $(LIBRARIES_$(compiler)): %: %.o common.hpp $(PATTERN_HASH).tmp pattern.hpp
	$(CXX) $(OPTLEVEL) $(LDFLAGS) ${${@}_LDFLAGS} $< -o $@

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
	