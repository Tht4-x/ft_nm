# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dancel <dancel@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/31 20:23:03 by dancel            #+#    #+#              #
#    Updated: 2026/08/31 20:23:06 by dancel           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Variables de Compilation
# ============================
NAME = libft.a
CC   = cc
FLAG = -Wall -Wextra -Werror -g3 -fPIC
LIBC = ar rcs

# Fichiers source
# ============================
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=objects/%.o)

# Cible principale
# ============================
all: create_objects_dirs ${NAME}

${NAME}: ${OBJS}
	${LIBC} ${NAME} ${OBJS}

create_objects_dirs:
	mkdir -p objects

objects/%.o: %.c
	$(CC) $(FLAG) -c $< -o $@

# Nettoyage des fichiers
# ============================
clean:
	rm -f objects/*.o
	rm -rf objects
	
fclean: clean
	rm -f ${NAME}

# Rebuild
# ============================
re: fclean all

# Cibles "fantômes"
# ============================
.PHONY: all clean fclean re
