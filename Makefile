# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/28 09:12:17 by nesdebie          #+#    #+#              #
#    Updated: 2025/04/29 09:32:39 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2

SRC_DIR = src
BUILD_DIR = build
EXT_DIR = external
GLM_DIR = $(EXT_DIR)/glm
SHADER_DIR = shaders

NAME = scop

GLSLANG_VALIDATOR = $(EXT_DIR)/glslangValidator

LIBS = -lvulkan -lglfw
INCLUDES = -I$(SRC_DIR) -I$(GLM_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

VERT_SHADER = $(SHADER_DIR)/triangle.vert.glsl
FRAG_SHADER = $(SHADER_DIR)/triangle.frag.glsl
SPV_VERT = $(SHADER_DIR)/triangle.vert.spv
SPV_FRAG = $(SHADER_DIR)/triangle.frag.spv

all: $(GLM_DIR) shaders $(BUILD_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(GLM_DIR):
	git clone https://github.com/g-truc/glm.git $(GLM_DIR)

$(SPV_VERT): $(VERT_SHADER) $(GLSLANG_VALIDATOR)
	$(GLSLANG_VALIDATOR) -V $< -o $@

$(SPV_FRAG): $(FRAG_SHADER) $(GLSLANG_VALIDATOR)
	$(GLSLANG_VALIDATOR) -V $< -o $@

shaders: $(SPV_VERT) $(SPV_FRAG)

clean:
	rm -rf $(BUILD_DIR) $(NAME) $(SPV_VERT) $(SPV_FRAG)

fclean: clean
	rm -rf $(GLM_DIR)

re: fclean all

rebuild: clean shaders $(BUILD_DIR) $(NAME)

.PHONY: all clean fclean re rebuild shaders
