SRC = tarinstall.c
OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wpedantic -Werror

LIB_ARCHIVE = -larchive

NAME = tarinstall

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(LIB_ARCHIVE)
	ln -fs "$(shell realpath $(NAME))" /usr/bin/$(NAME)

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
