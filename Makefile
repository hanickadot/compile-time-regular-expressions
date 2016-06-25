.PHONY: all clean
	
# -ftemplate-depth-1024 -ftemplate-backtrace-limit=0
CXXFLAGS := -std=c++1z -Wno-gnu-string-literal-operator-template 

all: math
	
math: math.o
	$(CXX) $(CXXFLAGS) -O3 math.o -o math

math.o: math.cpp static-parser.hpp
	$(CXX) $(CXXFLAGS) -O3 -c math.cpp -o math.o

clean:
	rm -f *.o math
