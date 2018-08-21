.PHONY: default all clean grammar

default: all
	
TARGETS := 

DESATOMAT := /www/root/desatomat/console/desatomat.php

CXXFLAGS := -std=c++17 -Iinclude -O3 -Wno-gnu-string-literal-operator-template
#-fconcepts

TESTS := $(wildcard tests/*.cpp)
TRUE_TARGETS := $(TARGETS:%.cpp=%)
OBJECTS := $(TARGETS:%.cpp=%.o) $(TESTS:%.cpp=%.o)
DEPEDENCY_FILES := $(TARGETS:%.cpp=%.d) $(TESTS:%.cpp=%.d)

all: $(TRUE_TARGETS) $(OBJECTS)
	
$(TRUE_TARGETS): %: %.o
	$(CXX) $(LDFLAGS) $< -o $@
	
$(OBJECTS): %.o: %.cpp
	time $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPEDENCY_FILES)

clean:
	rm -f $(TRUE_TARGETS) $(OBJECTS) $(DEPEDENCY_FILES)
	
grammar: include/ctre/pcre.hpp
	
regrammar: 
	rm -f include/ctre/pcre.hpp
	$(MAKE) grammar

include/ctre/pcre.hpp: include/ctre/pcre.gram
	$(DESATOMAT) --ll --q --input=include/ctre/pcre.gram --output=include/ctre/ --generator=cpp_ctre_v2  --cfg:fname=pcre.hpp --cfg:namespace=ctre --cfg:guard=CTRE__PCRE__HPP --cfg:grammar_name=pcre