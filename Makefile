CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2

SRC := src/main.cpp src/program.cpp src/memory.cpp src/generator.cpp
OBJ := $(SRC:.cpp=.o)

memscope: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) memscope

.PHONY: clean
