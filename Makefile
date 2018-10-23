.PHONY: default all clean grammar compare

default: all
	
TARGETS := result.cpp test.cpp $(wildcard tests/benchmark-exec/*.cpp)

DESATOMAT := /www/root/desatomat/console/desatomat.php

CXXFLAGS := -std=c++17 -Iinclude -O3 -Wno-gnu-string-literal-operator-template 
#-Xclang -fconcepts-ts
LDFLAGS := -lpcre2-8


TESTS := $(wildcard tests/*.cpp) $(wildcard tests/benchmark/*.cpp)
TRUE_TARGETS := $(TARGETS:%.cpp=%)
IGNORE := $(wildcard tests/benchmark/*.cpp) tests/benchmark-exec/boost.cpp wildcard tests/benchmark-exec/re2.cpp
OBJECTS_PRE := $(TARGETS:%.cpp=%.o) $(TESTS:%.cpp=%.o)
OBJECTS := $(filter-out $(IGNORE:%.cpp=%.o), $(OBJECTS_PRE))
DEPEDENCY_FILES := $(OBJECTS:%.o=%.d)

all: $(TRUE_TARGETS) $(OBJECTS)
	
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
	
grammar: include/ctre/pcre.hpp include/ctre/simple.hpp
	
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
	
REPEAT:=10

compare: mtent12.txt
	$(CXX) $(CXXFLAGS) -MMD -march=native -DPATTERN="\"(${PATTERN})\"" -c tests/benchmark-range/measurement.cpp -o tests/benchmark-range/measurement.o
	$(CXX) tests/benchmark-range/measurement.o -lboost_regex -lpcre2-8 -lre2 -o tests/benchmark-range/measurement
	tests/benchmark-range/measurement all mtent12.txt ${REPEAT}

#include/ctre/simple.hpp: include/ctre/simple.gram
#	@echo "LL1q $<"
#	@$(DESATOMAT) --ll --q --input=include/ctre/simple.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=simple.hpp --cfg:namespace=ctre --cfg:guard=CTRE__SIMPLE__HPP --cfg:grammar_name=simple
