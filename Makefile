# Define variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g
LDFLAGS = 
TARGET = softfoot-falls
SOURCES = ./SoftfootFalls/SoftfootFalls/SoftfootFalls.cpp ./SoftfootFalls/SoftfootFalls/Clock.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

# Pattern rule for compiling C++ source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove compiled files
clean:
	rm -f $(OBJECTS) $(TARGET)
