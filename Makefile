# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/09 10:40:22 by nesdebie          #+#    #+#              #
#    Updated: 2025/05/09 10:41:06 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2

# Project structure
SRC_DIR = src
BUILD_DIR = build
EXT_DIR = external
GLM_DIR = $(EXT_DIR)/glm
STB_HEADER = $(EXT_DIR)/stb_image.h
SHADER_DIR = shaders

# Executable name
NAME = scop

# External tools
GLSLANG_VALIDATOR = $(EXT_DIR)/glslangValidator

# Libraries and includes
LIBS = -lvulkan -lglfw
INCLUDES = -I$(SRC_DIR) -I$(GLM_DIR) -I$(EXT_DIR)

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Shader files
VERT_SHADER = $(SHADER_DIR)/triangle.vert.glsl
FRAG_SHADER = $(SHADER_DIR)/triangle.frag.glsl
SPV_VERT = $(SHADER_DIR)/triangle.vert.spv
SPV_FRAG = $(SHADER_DIR)/triangle.frag.spv

# Default target
all: $(GLM_DIR) $(STB_HEADER) shaders $(BUILD_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(GLM_DIR):
	git clone https://github.com/g-truc/glm.git $(GLM_DIR)

$(STB_HEADER):
	curl -L https://raw.githubusercontent.com/nothings/stb/master/stb_image.h -o $(STB_HEADER)

$(SPV_VERT): $(VERT_SHADER) $(GLSLANG_VALIDATOR)
	$(GLSLANG_VALIDATOR) -V $< -o $@

$(SPV_FRAG): $(FRAG_SHADER) $(GLSLANG_VALIDATOR)
	$(GLSLANG_VALIDATOR) -V $< -o $@

shaders: $(SPV_VERT) $(SPV_FRAG)

clean:
	rm -rf $(BUILD_DIR) $(NAME) $(SPV_VERT) $(SPV_FRAG)

fclean: clean
	rm -rf $(GLM_DIR)
	rm -f $(STB_HEADER)

re: fclean all

rebuild: clean shaders $(BUILD_DIR) $(NAME)

.PHONY: all clean fclean re rebuild shaders
