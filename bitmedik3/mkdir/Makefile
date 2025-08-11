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

SRC = shell.c shell_init.c shell_loop.c \
      lexer.c lexer_token.c lexer_extension.c lexer_utils.c lexer_check.c \
      parser.c parser_cmd.c parser_redir.c parser_expand.c parser_utils.c \
      execute.c execute_pipe.c execute_builtin.c execute_heredoc.c execute_utils.c execute_utils2.c execute_redir.c execute_process.c \
      builtin_echo.c builtin_cd.c builtin_export.c builtin_env.c builtin_exit.c \
      expander.c expander_utils.c expanded_dollar.c \
      env_basic.c env_init.c env_operations.c env_array.c env_free.c \
      export_basic.c export_operations.c export_utils.c \
      signal_handler.c signal_utils.c \
      gb_basic.c gb_main.c gb_env.c \
      exit_status.c \
      utils.c utils_str.c utils_str2.c utils_memory.c \
      process_heredoc.c process_heredoc2.c
 
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