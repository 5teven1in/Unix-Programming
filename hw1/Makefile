CXX		= g++
CXXFLAGS	= -Wall -g -std=c++17

PROGS	= hw1

all: $(PROGS)

%: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

.PHONY: clean test

clean:
	rm -f *~ $(PROGS)

test: $(PROGS)
	sudo ./$<
