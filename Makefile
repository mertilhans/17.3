NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror

LDFLAGS = -lreadline

SRC = shell.c lexer/lexer.c lexer/lexer_extension.c lexer/lexer_utils.c gb/gb.c lexer/utils.c parser.c execute.c builtin.c expander/expander.c env/env.c signal_handler.c
 
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
