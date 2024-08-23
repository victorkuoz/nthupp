CC = gcc
CXX = g++
LDLIBS = -lpng
CFLAGS = -lm -O3
# OPTFLAGS = -march=native -fopt-info-vec-all
hw2a: CFLAGS += -pthread
hw2b: CC = mpicc
hw2b: CXX = mpicxx
hw2b: CFLAGS += -fopenmp
CXXFLAGS = $(CFLAGS)
TARGETS = hw2a hw2seq

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS:=.o)