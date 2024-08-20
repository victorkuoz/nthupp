CC = mpicc
CXX = mpicxx
CFLAGS = -O3 -lm
CXXFLAGS = -O3 -lm
TARGETS = main

.PHONY: all gen trans clean

all: $(TARGETS)

gen:
	g++ -std=c++17 gen.cc -o gen

trans:
	g++ -std=c++17 trans.cc -o trans

clean:
	rm -f $(TARGETS) gen trans