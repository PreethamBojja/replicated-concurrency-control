# Makefile for compiling main.cpp

CXX = g++
CXXFLAGS = -std=c++11
TARGET = main
SRC = src/main.cpp
TESTS = $(shell ls test/* | sort -V) # Dynamically sorted in natural order

# Default target: build the project
all: $(TARGET)

# Compile the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run tests
test: $(TARGET)
	@echo "Running tests..."
	@for test_file in $(TESTS); do \
		echo "-----------------------------------------------------------------------------------------"; \
		echo ""; \
		echo "Running $$test_file:"; \
		echo "---------------------"; \
		./$(TARGET) $$test_file; \
		echo ""; \
	done

# Clean up compiled files
clean:
	rm -f $(TARGET)

# .PHONY to ensure clean runs even if there are files named clean or test
.PHONY: all clean test
