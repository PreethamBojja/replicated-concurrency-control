# Makefile for compiling main.cpp

CXX = g++
CXXFLAGS = -std=c++11
TARGET = main
SRC = src/main.cpp

# Default target: build the project
all: $(TARGET)

# Compile the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean up compiled files
clean:
	rm -f $(TARGET)

# .PHONY to ensure clean runs even if there are files named clean
.PHONY: all clean
