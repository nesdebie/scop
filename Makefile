# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/16 09:18:20 by nesdebie          #+#    #+#              #
#    Updated: 2025/04/16 09:18:21 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -I./src -I./glm
SRCDIR = src
OBJDIR = obj
GLMDIR = glm

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
NAME = scop

all: $(GLMDIR) $(NAME)

$(GLMDIR):
	@if [ ! -d "$(GLMDIR)" ]; then \
		echo "Cloning GLM library..."; \
		git clone https://github.com/g-truc/glm.git $(GLMDIR); \
	fi

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(GLMDIR)

re: fclean all

.PHONY: all clean fclean re