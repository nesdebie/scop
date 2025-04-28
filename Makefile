# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/28 09:12:17 by nesdebie          #+#    #+#              #
#    Updated: 2025/04/28 09:14:56 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2

SRC_DIR = src
BUILD_DIR = build
EXT_DIR = external
GLM_DIR = $(EXT_DIR)/glm

LIBS = -lvulkan -lglfw
INCLUDES = -I$(SRC_DIR) -I$(GLM_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

NAME = scop

all: $(GLM_DIR) $(BUILD_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(GLM_DIR):
	mkdir -p $(EXT_DIR)
	git clone https://github.com/g-truc/glm.git $(GLM_DIR)

clean:
	rm -rf $(BUILD_DIR) $(NAME)

fclean: clean
	rm -rf $(EXT_DIR)

re: fclean all

recompile: clean all

.PHONY: all clean fclean re recompile
