.PHONY: all clean
	
# -ftemplate-depth-1024 -ftemplate-backtrace-limit=0
override CXXFLAGS += -std=c++1z -Wno-gnu-string-literal-operator-template -O3

override TARGETS := math regexp

all: $(TARGETS)
	
math: math.o 
	$(CXX) $(CXXFLAGS) $< -o $@
	
regexp: regexp.o 
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.cpp static-parser.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)
