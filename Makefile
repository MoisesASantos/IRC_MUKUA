# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/09 11:55:31 by mosantos          #+#    #+#              #
#    Updated: 2026/08/06 15:18:50 by sgaspar          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXX = c++

SRC_DIR = src

NAME = ircserv
SRCS = $(SRC_DIR)/Client/Client.cpp \
	   $(SRC_DIR)/Server/Server.cpp \
	   $(SRC_DIR)/Server/ServerHandler.cpp \
	   $(SRC_DIR)/channel/Channel.cpp \
	   $(SRC_DIR)/ft_engine.cpp \
	   $(SRC_DIR)/main.cpp

BONUS_NAME = ircbot
BONUS_SRCS = $(SRC_DIR)/Bot/Bot.cpp \
		   $(SRC_DIR)/main_bonus.cpp

HEADERS = $(SRC_DIR)/Headers/header.hpp \
		  $(SRC_DIR)/Headers/includes.hpp \
		  $(SRC_DIR)/Client/Client.hpp \
		  $(SRC_DIR)/Server/Server.hpp \
		  $(SRC_DIR)/Server/ServerHandler.hpp \
		  $(SRC_DIR)/channel/Channel.hpp
		  
OBJS = $(SRCS:.cpp=.o)
BONUS_OBJS = $(BONUS_SRCS:.cpp=.o)

all: $(NAME)

bonus: $(BONUS_NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CXX) $(CXXFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS)

$(OBJS): $(HEADERS)

$(BONUS_OBJS): $(HEADERS) $(SRC_DIR)/Bot/Bot.hpp

clean:
	rm -f $(OBJS) $(BONUS_OBJS) $(BONUS_NAME)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
