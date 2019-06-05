CXX		= g++
CXXFLAGS	= -shared -fPIC -std=c++17
LDL		= -ldl

PROGS		= fsmon.so

all: $(PROGS)

%.so: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDL)

.PHONY: clean

clean:
	rm -f *~ $(PROGS)
