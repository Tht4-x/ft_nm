# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dancel <dancel@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/31 20:16:51 by dancel            #+#    #+#              #
#    Updated: 2026/09/05 19:31:40 by dancel           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# NAME
# ============================

NAME      = ft_nm

# GENERIC
# ============================

CC        = cc
CFLAGS    = -Wall -Wextra -Werror -g3
INCLUDES  = -I includes

SRC_DIR   = srcs
OBJ_DIR   = objects
SRCS      = $(wildcard $(SRC_DIR)/*.c)
OBJS      = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# TARGET
# ============================

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# CLEANING
# ============================
clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)


# REBUILD
# ============================
re: fclean all

# GHOST
# ============================
.PHONY: all clean fclean re test

