CXX	= g++
CFLAGS	= -shared -fPIC -std=c++17
LDL	= -ldl

PROGS	= fsmon.so

all: $(PROGS)

%.so: %.cpp
	$(CXX) -o $@ $(CFLAGS) $^ $(LDL)

.PHONY: clean

clean:
	rm -f *~ $(PROGS)
