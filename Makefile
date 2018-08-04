.PHONY: default all clean 

default: all
	
TARGETS := test.cpp

CXXFLAGS := -std=c++17 -Iinclude -O3 -Wno-gnu-string-literal-operator-template

TRUE_TARGETS := $(TARGETS:%.cpp=%)
OBJECTS := $(TARGETS:%.cpp=%.o)
DEPEDENCY_FILES := $(TARGETS:%.cpp=%.d)

all: $(TRUE_TARGETS)
	
$(TRUE_TARGETS): %: %.o
	$(CXX) $(LDFLAGS) $< -o $@
	
$(OBJECTS): %.o: %.cpp
	time $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPEDENCY_FILES)

clean:
	rm -f $(TRUE_TARGETS) $(OBJECTS) $(DEPEDENCY_FILES)