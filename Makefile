# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/09 11:55:31 by mosantos          #+#    #+#              #
#    Updated: 2026/07/22 15:45:26 by mosantos         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXX = c++

SRC_DIR = src

NAME = ircserv
SRCS = $(SRC_DIR)/main.c \
	   $(SRC_DIR)/Client/Client.cpp \
	   $(SRC_DIR)/Client/Server.cpp


HEADERS = includes.hpp \
		  $(SRC_DIR)/Client/Client.hpp \
		  $(SRC_DIR)/Server/Server.hpp
		  
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS): $(HEADERS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
