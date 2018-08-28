.PHONY: default all clean grammar

default: all
	
TARGETS := result.cpp test.cpp $(wildcard tests/benchmark-exec/*.cpp)

DESATOMAT := /www/root/desatomat/console/desatomat.php

CXXFLAGS := -std=c++17 -Iinclude -O3 -Wno-gnu-string-literal-operator-template
LDFLAGS := -L/usr/local/Cellar/pcre2/10.31/lib -lpcre2-8
#-fconcepts

TESTS := $(wildcard tests/*.cpp) $(wildcard tests/benchmark/*.cpp)
TRUE_TARGETS := $(TARGETS:%.cpp=%)
IGNORE := $(wildcard tests/benchmark/many-of-*.cpp)
OBJECTS_PRE := $(TARGETS:%.cpp=%.o) $(TESTS:%.cpp=%.o)
OBJECTS := $(filter-out $(IGNORE:%.cpp=%.o), $(OBJECTS_PRE))
DEPEDENCY_FILES := $(OBJECTS:%.o=%.d)

all: $(TRUE_TARGETS) $(OBJECTS)
	
$(TRUE_TARGETS): %: %.o
	$(CXX)  $< $(LDFLAGS) -o $@
	
$(OBJECTS): %.o: %.cpp
	time $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPEDENCY_FILES)

benchmark:
	@$(MAKE) clean
	@$(MAKE) IGNORE=""
	
benchmark-clean:
	@$(MAKE) IGNORE="" clean

clean:
	rm -f $(TRUE_TARGETS) $(OBJECTS) $(DEPEDENCY_FILES)
	
grammar: include/ctre/pcre.hpp include/ctre/simple.hpp
	
regrammar: 
	@rm -f include/ctre/pcre.hpp
	@$(MAKE) grammar
	
include/ctre/pcre.hpp: include/ctre/pcre.gram
	@echo "LL1q $<"
	@$(DESATOMAT) --ll --q --input=include/ctre/pcre.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=pcre.hpp --cfg:namespace=ctre --cfg:guard=CTRE__PCRE__HPP --cfg:grammar_name=pcre

#include/ctre/simple.hpp: include/ctre/simple.gram
#	@echo "LL1q $<"
#	@$(DESATOMAT) --ll --q --input=include/ctre/simple.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=simple.hpp --cfg:namespace=ctre --cfg:guard=CTRE__SIMPLE__HPP --cfg:grammar_name=simple