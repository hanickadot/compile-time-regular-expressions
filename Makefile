.PHONY: all clean
	
# -ftemplate-depth-1024 -ftemplate-backtrace-limit=0
override CXXFLAGS += -std=c++1z -Wno-gnu-string-literal-operator-template -O3

all: math
	
math: math.o
	$(CXX) $(CXXFLAGS) math.o -o math

math.o: math.cpp static-parser.hpp
	$(CXX) $(CXXFLAGS) -c math.cpp -o math.o

clean:
	rm -f *.o math
