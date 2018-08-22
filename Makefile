.PHONY: default all clean grammar

default: all
	
TARGETS := 

DESATOMAT := /www/root/desatomat/console/desatomat.php

CXXFLAGS := -std=c++17 -Iinclude -O3 -Wno-gnu-string-literal-operator-template
#-fconcepts

TESTS := $(wildcard tests/*.cpp) $(wildcard tests/benchmark/*.cpp)
TRUE_TARGETS := $(TARGETS:%.cpp=%)
IGNORE := $(wildcard tests/benchmark/*.cpp)
OBJECTS_PRE := $(TARGETS:%.cpp=%.o) $(TESTS:%.cpp=%.o)
OBJECTS := $(filter-out $(IGNORE:%.cpp=%.o), $(OBJECTS_PRE))
DEPEDENCY_FILES := $(OBJECTS:%.o=%.d)

all: $(TRUE_TARGETS) $(OBJECTS)
	
$(TRUE_TARGETS): %: %.o
	$(CXX) $(LDFLAGS) $< -o $@
	
$(OBJECTS): %.o: %.cpp
	time $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPEDENCY_FILES)

benchmark:
	$(MAKE) clean
	$(MAKE) IGNORE=""

clean:
	rm -f $(TRUE_TARGETS) $(OBJECTS) $(DEPEDENCY_FILES)
	
grammar: include/ctre/pcre2.hpp include/ctre/pcre.hpp
	
regrammar: 
	rm -f include/ctre/pcre2.hpp include/ctre/pcre.hpp
	$(MAKE) grammar
	
include/ctre/pcre.hpp: include/ctre/pcre.gram
	$(DESATOMAT) --ll --q --input=include/ctre/pcre.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=pcre.hpp.pre --cfg:namespace=ctre --cfg:guard=CTRE__PCRE__HPP --cfg:grammar_name=pcre
	cat include/ctre/pcre.hpp.pre | sed -e 's/ctll::/ctre::/g' > include/ctre/pcre.hpp
	rm -f include/ctre/pcre.hpp.pre

include/ctre/pcre2.hpp: include/ctre/pcre.gram
	$(DESATOMAT) --ll --q --input=include/ctre/pcre.gram --output=include/ctre/ --generator=cpp_ctll_v2  --cfg:fname=pcre2.hpp --cfg:namespace=ctre --cfg:guard=CTRE__PCRE2__HPP --cfg:grammar_name=pcre2