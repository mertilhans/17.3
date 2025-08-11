NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror

LDFLAGS = -lreadline


RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[1;37m
NC = \033[0m

SRC = shell.c lexer.c lexer_extension.c lexer_utils.c free.c utils.c parser.c execute.c builtin_basic.c builtin_export.c expander.c env_basic.c env_init.c env_operations.c env_array.c env_free.c export_basic.c export_operations.c signal_handler.c gb_basic.c gb_main.c gb_env.c exit_status.c
 
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(YELLOW)🔗 Linking object files...$(NC)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)✅ $(NAME) compiled successfully!$(NC)"

%.o: %.c
	@echo "$(BLUE)🔨 Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🧹 Cleaning object files...$(NC)"
	@rm -rf $(OBJ)
	@echo "$(GREEN)✅ Object files cleaned!$(NC)"

fclean: clean
	@echo "$(YELLOW)🔥 Removing executable...$(NC)"
	@rm -rf $(NAME)
	@echo "$(GREEN)✅ Full clean completed!$(NC)"

re: fclean all

.PHONY: all clean fclean re
