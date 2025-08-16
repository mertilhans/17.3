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

SRC = shell.c lexer.c lexer_extension.c lexer_utils.c utils.c  builtin_export.c env_basic.c env_init.c env_operations.c env_array.c env_free.c export_basic.c export_operations.c export_utils.c export_process.c signal_handler.c gb_basic.c gb_main.c gb_env.c exit_status.c \
execute_builtin.c execute_fd.c execute_error_exit.c execute_redirections.c execute_init.c execute_child.c execute_utils.c execute_main.c execute_fork.c execute_finder.c \
heredoc_utils.c heredoc_process.c heredoc_loop.c heredoc_read.c \
parser_init.c parser_redirect.c parser_heredoc.c parser_handle.c parser_main.c parser_syntax.c parser_argv.c \
expander_utils.c expander_variable.c expander_quotes.c expander_quotes2.c expander_heredoc.c expander_simple.c expander_utils2.c \
builtin_cd.c builtin_echo.c builtin_env.c builtin_exit.c builtin_pwd.c builtin_unset.c signal_utils.c
 
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
