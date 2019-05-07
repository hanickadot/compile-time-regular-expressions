.PHONY: default all run clean header

PATTERN := ABCD|DEFGH|EFGHI|A{4,}
FILE := input.txt

TEST_CASE := unknown

PATTERN_HASH := $(shell echo "$(PATTERN)" | /sbin/md5)

LIBRARIES := $(sort baseline ctre ctfa pcre pcre-jit srell re2 boost xpressive xpressive-ct std)

#LIBRARIES_clang = re2 boost xpressive xpressive-ct

default: all
	
compiler := $(shell $(CXX) -v 2>&1 | grep -E "^clang|^gcc" | head -n1 | sed -E "s/([a-z]+).*/\\1/")

ifeq ($(compiler),clang)
CXXFLAGS_ADDITIONAL := -fconstexpr-steps=100000000 
ifeq ($(shell uname),Darwin)
CXXFLAGS_ADDITIONAL += -isysroot  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -I../../include -Isrell_include -I/usr/local/Cellar/pcre2/10.33/include -I/usr/local/Cellar/re2/20180801/include -I/usr/local/Cellar/boost/1.69.0_2/include
DYLD_PATH :=
endif
else
CXXFLAGS_ADDITIONAL += -I/Users/hanka/Downloads/re2-2019-04-01 -I../../include -Isrell_include -Isrell_include -I/usr/local/Cellar/pcre2/10.33/include -I/Users/hanka/Downloads/boost_1_70_0
LDFLAGS += -L/Users/hanka/Downloads/re2-2019-04-01/hana-build/
DYLD_PATH := DYLD_LIBRARY_PATH=/Users/hanka/Downloads/boost_1_70_0/stage/lib
endif

OPTLEVEL := -O3

CXXFLAGS := $(CXXFLAGS_ADDITIONAL) -std=c++2a 
LDFLAGS :=

ctre_CXXFLAGS := 
ctre_LDFLAGS := 
ctre_NAME := CTRE

baseline_NAME := baseline

ctfa_NAME := CTFA
ctfa-data_NAME := CTFA (data-driven)

ifeq ($(compiler),gcc)
boost_CXXFLAGS := 
boost_LDFLAGS := -L/Users/hanka/Downloads/boost_1_70_0/stage/lib -lboost_regex
else
boost_CXXFLAGS := 
boost_LDFLAGS := -lboost_regex 
endif
boost_NAME := boost::regex

pcre_CXXFLAGS := 
pcre_LDFLAGS := -L/usr/local/Cellar/pcre2/10.33/lib -lpcre2-8
pcre_NAME := PCRE2

pcre-jit_CXXFLAGS :=
pcre-jit_LDFLAGS := -L/usr/local/Cellar/pcre2/10.33/lib -lpcre2-8
pcre-jit_NAME := PCRE2 (jit)

re2_CXXFLAGS :=
ifeq ($(compiler),gcc)
re2_LDFLAGS := -lre2 -L/Users/hanka/Downloads/re2-2019-04-01/hana-build/
else
re2_LDFLAGS := -lre2 
endif
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
	time $(CXX) $(OPTLEVEL) $(CXXFLAGS) ${${@:%.o=%}_CXXFLAGS} -c $< -o $@

$(LIBRARIES) $(LIBRARIES_$(compiler)): %: %.o common.hpp $(PATTERN_HASH).tmp pattern.hpp
	$(CXX) $(OPTLEVEL) $(LDFLAGS) ${${@}_LDFLAGS} $< -o $@

header.csv:
	echo "library;pattern;duration" > header.csv
	echo "library;pattern;duration" > result.csv

$(LIBRARIES:%=%.run): %.run: % header
	$(DYLD_PATH) ./$< $(FILE) benchmark ${TEST_CASE} 2>/dev/null >>result.csv
	touch $@

run: $(LIBRARIES:%=%.run)
	
node:
	node node-v8.js path=${FILE} pattern="${PATTERN}"

jsc:
	jsc jsc.js -- path=${FILE} pattern="${PATTERN}"

result.csv: header.csv $(LIBRARIES:%=%.run)
	touch result.csv

clean:
	rm -f $(LIBRARIES) *.tmp header.csv result.csv
	