CC = c++

SRC_DIR = ./src
OBJ_DIR = $(SRC_DIR)/obj

CPPFILES = \
	main.cpp \
	Server.cpp \
	Client.cpp \
	Channel.cpp \
	Errors.cpp

SRCS = $(addprefix $(SRC_DIR)/, $(CPPFILES))
OBJS = $(addprefix $(OBJ_DIR)/, $(CPPFILES:.cpp=.o))

INCLUDE = -I./include
FLAGS = -Wall -Wextra -g -std=c++17 $(INCLUDE)	#-Werror
NAME = ircserv

$(NAME) : $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\033[32m$(NAME) created\033[0m"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(FLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

all: $(NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[33mRemoved $(OBJ_DIR)\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[33mRemoved $(NAME)\033[0m"

re: fclean all

.PHONY: all, clean, fclean, re
