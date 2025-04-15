# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/15 20:43:15 by nesdebie          #+#    #+#              #
#    Updated: 2025/04/15 20:43:16 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = viewer

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

LIBS = -lglfw -ldl -lGL -lX11 -lpthread

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

.PHONY: clean fclean re
