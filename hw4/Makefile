CXX		= g++
CXXFLAGS	= -g -Wall -std=c++17
LIBS		= -lelf -lcapstone

PROGS		= sdb

all: $(PROGS)

$(PROGS): $(wildcard *.cpp)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS)

.PHONY: clean

clean:
	rm -f *~ $(PROGS)
