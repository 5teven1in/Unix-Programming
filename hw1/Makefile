CXX	= g++
CFLAGS	= -Wall -g -std=c++17

PROGS	= hw1

all: $(PROGS)

%: %.cpp
	$(CXX) -o $@ $(CFLAGS) $< $(LDFLAGS)

.PHONY: clean test

clean:
	rm -f *~ $(PROGS)

test: $(PROGS)
	sudo ./$<
