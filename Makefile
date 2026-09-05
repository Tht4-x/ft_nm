# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dancel <dancel@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/31 20:16:51 by dancel            #+#    #+#              #
#    Updated: 2026/09/05 19:25:23 by dancel           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# NAME
# ============================

NAME      = ft_nm

# GENERIC
# ============================

CC        = cc
CFLAGS    = -Wall -Wextra -Werror -g3
INCLUDES  = -I includes -I libft

LIBFT_DIR = libft
LIBFT     = $(LIBFT_DIR)/libft.a

SRC_DIR   = srcs
OBJ_DIR   = objects
SRCS      = $(wildcard $(SRC_DIR)/*.c)
OBJS      = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# TARGET
# ============================

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJS) $(LIBFT)
	@ln -sf $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	@$(MAKE) -s -C $(LIBFT_DIR)

# CLEANING
# ============================
clean:
	@$(MAKE) clean -s -C $(LIBFT_DIR)
	rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) fclean -s -C $(LIBFT_DIR)
	rm -f $(NAME) $(LINKNAME)


# REBUILD
# ============================
re: fclean all

# GHOST
# ============================
.PHONY: all clean fclean re test

