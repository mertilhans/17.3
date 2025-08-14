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

SRC = shell.c shell_init.c shell_loop.c shell_utils.c shell_process.c \
      lexer.c lexer_token.c lexer_utils.c lexer_extract.c lexer_validate.c lexer_helpers.c \
      parser.c parser_redir.c  parser_argv.c parser_utils.c \
      execute.c execute_pipe.c execute_builtin.c execute_redir.c execute_heredoc.c \
      builtin_echo.c builtin_cd.c builtin_pwd.c builtin_exit.c builtin_env.c \
      builtin_export.c builtin_export_utils.c builtin_unset.c \
      expander.c expander_utils.c expander_heredoc.c expander_dollar.c expander_split.c \
      env_basic.c env_init.c env_operations.c env_array.c env_utils.c \
      export_basic.c export_operations.c export_utils.c \
      gb_basic.c gb_main.c gb_env.c gb_utils.c gb_free.c \
      signal_handler.c signal_utils.c signal_heredoc.c \
      exit_status.c utils.c utils_str.c utils_mem.c utils_convert.c \
      heredoc_read.c heredoc_process.c heredoc_utils.c heredoc_expand.c \
      execute_utils.c execute_error.c execute_fork.c execute_child.c \
      parser_helpers.c env_gc.c

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