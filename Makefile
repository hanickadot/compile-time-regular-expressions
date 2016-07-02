.PHONY: all clean
	
# -ftemplate-depth-1024 -ftemplate-backtrace-limit=0
override CXXFLAGS += -std=c++1z -Wno-gnu-string-literal-operator-template -O3

override TARGETS := math regexp pregexp code-size-test catch-regexp-example

all: $(TARGETS)
	
math: math.o 
	$(CXX) $(CXXFLAGS) $< -o $@
	
regexp: regexp.o 
	$(CXX) $(CXXFLAGS) $< -o $@
	
pregexp: pregexp.o 
	$(CXX) $(CXXFLAGS) $< -o $@
	
number: number.o
	$(CXX) $(CXXFLAGS) $< -o $@
	
code-size-test: code-size-test.o
	$(CXX) $(CXXFLAGS) $< -o $@
	
catch-regexp-example: catch-regexp-example.o
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.cpp $(wildcard *.hpp */*.hpp)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)
