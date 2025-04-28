# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Paths
SRC_DIR = src
BUILD_DIR = build
EXT_DIR = external
GLM_DIR = $(EXT_DIR)/glm

# Libraries
LIBS = -lvulkan -lglfw
INCLUDES = -I$(SRC_DIR) -I$(GLM_DIR)

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Executable
TARGET = scop

# Default target
all: $(GLM_DIR) $(BUILD_DIR) $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clone GLM if it doesn't exist
$(GLM_DIR):
	mkdir -p $(EXT_DIR)
	git clone https://github.com/g-truc/glm.git $(GLM_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Full clean: also remove external libraries
fclean: clean
	rm -rf $(EXT_DIR)

re: fclean all

.PHONY: all clean fclean re
