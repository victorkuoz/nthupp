CC = mpicc
CXX = mpicxx
CFLAGS = -O3 -lm
CXXFLAGS = -O3 -lm
TARGETS = main

.PHONY: all gen clean

all: $(TARGETS)

gen:
	g++ -std=c++17 gen.cc -o gen

clean:
	rm -f $(TARGETS) gen