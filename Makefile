# Define variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g
LDFLAGS = 
TARGET = softfoot-falls
SOURCES = \
    ./SoftfootFalls/SoftfootFalls/SoftfootFalls.cpp \
	./SoftfootFalls/SoftfootFalls/Time.cpp \
	./SoftfootFalls/SoftfootFalls/Load.cpp \
	./SoftfootFalls/SoftfootFalls/Node.cpp \
	./SoftfootFalls/SoftfootFalls/SDLInterface.cpp \
	./SoftfootFalls/SoftfootFalls/SDLWrapper.cpp \
	./SoftfootFalls/SoftfootFalls/Window.cpp \
	./SoftfootFalls/SoftfootFalls/Texture.cpp \
	./SoftfootFalls/SoftfootFalls/Player.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET) -L/usr/lib -lSDL2 -lSDL2_image -lSDL2_mixer

# Pattern rule for compiling C++ source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove compiled files
clean:
	rm -f $(OBJECTS) $(TARGET)
