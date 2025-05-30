# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/09 10:40:22 by nesdebie          #+#    #+#              #
#    Updated: 2025/05/30 10:01:45 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2

SRC_DIR = src
BUILD_DIR = build
EXT_DIR = external
GLM_DIR = $(EXT_DIR)/glm
STB_HEADER = $(EXT_DIR)/stb_image.h
SHADER_DIR = shaders

NAME = scop

VULKAN_SDK_VERSION = 1.4.313.0
GLSLC = $(EXT_DIR)/glslc
SDK_GLSLC_PATH = $(VULKAN_SDK_VERSION)/x86_64/bin/glslc

LIBS = -lvulkan -lglfw
INCLUDES = -I$(SRC_DIR) -I$(GLM_DIR) -I$(EXT_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

VERT_SHADER = $(SHADER_DIR)/scop.vert.glsl
FRAG_SHADER = $(SHADER_DIR)/scop.frag.glsl
SPV_VERT = $(SHADER_DIR)/scop.vert.spv
SPV_FRAG = $(SHADER_DIR)/scop.frag.spv

all: $(GLM_DIR) $(STB_HEADER) shaderc-install shaders $(BUILD_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(GLM_DIR):
	mkdir -p $(EXT_DIR)
	git clone https://github.com/g-truc/glm.git $(GLM_DIR)

$(STB_HEADER):
	curl -L https://raw.githubusercontent.com/nothings/stb/master/stb_image.h -o $(STB_HEADER)

$(SPV_VERT): $(VERT_SHADER) $(GLSLC)
	$(GLSLC) -fshader-stage=vert $< -o $@

$(SPV_FRAG): $(FRAG_SHADER) $(GLSLC)
	$(GLSLC) -fshader-stage=frag $< -o $@


shaders: $(SPV_VERT) $(SPV_FRAG)

shaderc-install:
	curl -L "https://sdk.lunarg.com/sdk/download/$(VULKAN_SDK_VERSION)/linux/vulkan-sdk-$(VULKAN_SDK_VERSION)-linux.tar.xz?Human=true" \
	     -o $(EXT_DIR)/vulkan-sdk.tar.xz
	tar -xf $(EXT_DIR)/vulkan-sdk.tar.xz -C $(EXT_DIR) $(SDK_GLSLC_PATH)
	mv $(EXT_DIR)/$(SDK_GLSLC_PATH) $(GLSLC)
	rm -f $(EXT_DIR)/vulkan-sdk.tar.xz
	rm -rf $(EXT_DIR)/$(VULKAN_SDK_VERSION)


clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf $(NAME) $(SPV_VERT) $(SPV_FRAG) $(EXT_DIR)

re: fclean all

rebuild: clean shaders $(BUILD_DIR) $(NAME)

.PHONY: all clean fclean re rebuild shaders shaderc-install
