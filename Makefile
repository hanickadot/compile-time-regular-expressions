.PHONY: default all clean grammar compare single-header single-header/ctre.hpp single-header/ctre-unicode.hpp single-header/unicode-db.hpp

default: all
	
TARGETS := $(wildcard tests/benchmark-exec/*.cpp)
IGNORE := $(wildcard tests/benchmark/*.cpp) $(wildcard tests/benchmark-exec/*.cpp)

DESATOMAT := /bin/false

CXX_STANDARD := 20

PEDANTIC:=-pedantic

override CXXFLAGS := $(CXXFLAGS) -std=c++$(CXX_STANDARD) -Iinclude -O3 $(PEDANTIC) -Wall -Wextra -Werror -Wconversion
LDFLAGS := 

TESTS := $(wildcard tests/*.cpp) $(wildcard tests/benchmark/*.cpp)
TRUE_TARGETS := $(TARGETS:%.cpp=%)
override TRUE_TARGETS := $(filter-out $(IGNORE:%.cpp=%), $(TRUE_TARGETS))
OBJECTS := $(TARGETS:%.cpp=%.o) $(TESTS:%.cpp=%.o)
override OBJECTS := $(filter-out $(IGNORE:%.cpp=%.o),$(OBJECTS))
DEPEDENCY_FILES := $(OBJECTS:%.o=%.d)

all: $(TRUE_TARGETS) $(OBJECTS)
	
list:
	echo $(SUPPORTED_CPP20)
	
$(TRUE_TARGETS): %: %.o
	$(CXX)  $< $(LDFLAGS) -o $@ 
	
$(OBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPEDENCY_FILES)

benchmark:
	@$(MAKE) clean
	@$(MAKE) IGNORE=""
	
benchmark-clean:
	@$(MAKE) IGNORE="" clean

clean:
	rm -f $(TRUE_TARGETS) $(OBJECTS) $(DEPEDENCY_FILES) mtent12.txt mtent12.zip
	
grammar: include/ctre/pcre.hpp
	
regrammar: 
	@rm -f include/ctre/pcre.hpp
	@$(MAKE) grammar
	
include/ctre/pcre.hpp: include/ctre/pcre.gram
	@echo "LL1q $<"
	@$(DESATOMAT) --ll --q --input=include/ctre/pcre.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=pcre.hpp --cfg:namespace=ctre --cfg:guard=CTRE__PCRE__HPP --cfg:grammar_name=pcre
	
mtent12.zip:
	curl -s http://www.gutenberg.org/files/3200/old/mtent12.zip -o mtent12.zip
	
mtent12.txt: mtent12.zip
	unzip -o mtent12.zip
	touch mtent12.txt

single-header: single-header/ctre.hpp single-header/ctre-unicode.hpp single-header/unicode-db.hpp

single-header/unicode-db.hpp: include/unicode-db/unicode-db.hpp
	cp $+ $@

single-header/ctre.hpp:
	python3 -m quom include/ctre.hpp ctre.hpp.tmp
	echo "/*" > single-header/ctre.hpp
	cat LICENSE >> single-header/ctre.hpp
	echo "*/" >> single-header/ctre.hpp
	cat ctre.hpp.tmp >> single-header/ctre.hpp
	rm ctre.hpp.tmp

single-header/ctre-unicode.hpp:
	python3 -m quom include/ctre-unicode.hpp ctre-unicode.hpp.tmp
	echo "/*" > single-header/ctre-unicode.hpp
	cat LICENSE >> single-header/ctre-unicode.hpp
	echo "*/" >> single-header/ctre-unicode.hpp
	cat ctre-unicode.hpp.tmp >> single-header/ctre-unicode.hpp
	rm ctre-unicode.hpp.tmp
	
REPEAT:=10

compare: mtent12.txt
	$(CXX) $(CXXFLAGS) -MMD -march=native -DPATTERN="\"(${PATTERN})\"" -c tests/benchmark-range/measurement.cpp -o tests/benchmark-range/measurement.o
	$(CXX) tests/benchmark-range/measurement.o -lboost_regex -lpcre2-8 -lre2 -o tests/benchmark-range/measurement
	tests/benchmark-range/measurement all mtent12.txt ${REPEAT}
