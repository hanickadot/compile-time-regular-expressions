.PHONY: default all clean single-header run
	
default: all

compiler := $(shell $(CXX) -v 2>&1 | grep -E "^clang|^gcc" | head -n1 | sed -E "s/([a-z]+).*/\\1/")

ifeq ($(compiler),clang)
CXXFLAGS_ADDITIONAL := -fconstexpr-steps=12712420
endif

CXXFLAGS := -O3 -std=c++2a -Iinclude -isysroot  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -Wall -Wextra -pedantic 
LDFLAGS := 

override ALL_HEADERS := $(wildcard include/*.hpp include/*/*.hpp include/*/*/*.hpp)

override TESTS_SOURCES := $(wildcard tests/*.cpp tests/exec/*.cpp)
override TESTS_OBJECTS := $(TESTS_SOURCES:%.cpp=%.o)
override TESTS_EXEC := $(filter tests/exec/%, $(TESTS_OBJECTS:%.o=%))
override TESTS_EXEC_RUN := $(TESTS_EXEC:%=%.stamp)
override DEP_FILES =  $(wildcard *.d */*.d */*/*.d)

-include $(TESTS_OBJECTS:%.o=%.d)

all: $(TESTS_OBJECTS) $(TESTS_EXEC) $(TESTS_EXEC_RUN)

run: $(TESTS_EXEC_RUN)

$(TESTS_OBJECTS): %.o: %.cpp Makefile
	time $(CXX) -MMD $(CXXFLAGS) -c $< -o $@

$(TESTS_EXEC): %: %.o Makefile
	$(CXX) $(LDFLAGS) $< -o $@

$(TESTS_EXEC_RUN): %.stamp: % %.o Makefile
	$< && touch $@

single-header/ctfa.hpp: ${ALL_HEADERS}
	python3.7 -m quom include/ctfa.hpp ctfa.hpp.tmp
	echo "/*\n" > $@
	cat LICENSE >> $@
	echo "\n*/\n" >> $@
	cat ctfa.hpp.tmp >> $@
	rm ctfa.hpp.tmp

single-header: single-header/ctfa.hpp

clean: 
	rm -f $(TESTS_OBJECTS) $(TESTS_EXEC) $(TESTS_EXEC_RUN) $(DEP_FILES)