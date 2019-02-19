.PHONY: default all clean grammar compare single-header single-header/ctre.hpp

default: all
	
TARGETS := $(wildcard tests/benchmark-exec/*.cpp)
IGNORE := $(wildcard tests/benchmark/*.cpp) $(wildcard tests/benchmark-exec/*.cpp)

DESATOMAT := /www/root/desatomat/console/desatomat.php

CPP_STANDARD := $(shell ./cpp-20-check.sh $(CXX))

CXXFLAGS := $(CPP_STANDARD) -Iinclude -O3 -pedantic -Wall -Wextra 
LDFLAGS := -lre2 -lboost_regex -lpcre2-8 

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
	time $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

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

single-header: single-header/ctre.hpp

single-header/ctre.hpp:
	python3.7 -m quom include/ctre.hpp ctre.hpp.tmp
	echo "/*\n" > single-header/ctre.hpp
	cat LICENSE >> single-header/ctre.hpp
	echo "\n*/\n" >> single-header/ctre.hpp 
	cat ctre.hpp.tmp >> single-header/ctre.hpp
	rm ctre.hpp.tmp
	
REPEAT:=10

compare: mtent12.txt
	$(CXX) $(CXXFLAGS) -MMD -march=native -DPATTERN="\"(${PATTERN})\"" -c tests/benchmark-range/measurement.cpp -o tests/benchmark-range/measurement.o
	$(CXX) tests/benchmark-range/measurement.o -lboost_regex -lpcre2-8 -lre2 -o tests/benchmark-range/measurement
	tests/benchmark-range/measurement all mtent12.txt ${REPEAT}
