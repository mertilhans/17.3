#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/stat.h>
# include <errno.h>
# include <ctype.h>
# include <stdint.h>

// ==================== STRUCTURES ====================

typedef struct s_heredoc_data
{
	int		last_heredoc_fd;
	size_t	heredoc_len;
	char	*heredoc_content;
}	t_heredoc_data;

typedef struct s_exec_data
{
	int		in_fd;
	int		pipefd[2];
	int		*pids;
	int		i;
	int		pids_size;
	char	**env;
	int		original_stdin;
	int		original_stdout;
}	t_exec_data;

typedef struct s_heredoc_buffer
{
	char	*content;
	char	*line;
	char	*line_with_nl;
	char	*new_content;
	size_t	delimiter_len;
}	t_heredoc_buffer;

typedef struct s_token_data
{
	char	*word;
	char	in_quote;
	int		len;
	int		capacity;
}	t_token_data;

typedef struct s_gb
{
	void			*data;
	struct s_gb		*next;
}	t_gb;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_EOF,
	TOKEN_ERROR
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_export
{
	char				*key;
	char				*value;
	struct s_export		*next;
}	t_export;

typedef struct s_tokenizer
{
	char	*input;
	int		pos;
	int		len;
	char	current;
}	t_tokenizer;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

typedef struct s_redirection
{
	char					*filename;
	t_redir_type			type;
	int						no_expand;
	struct s_redirection	*next;
}	t_redirection;

typedef struct s_parser
{
	int					fd_i;
	int					fd_o;
	char				**argv;
	int					argv_cap;
	t_redirection		*redirs;
	struct s_parser		*next;
	int					parse_error;
	int					heredoc_fd;
}	t_parser;

typedef struct s_all
{
	t_parser	*cmd_list;
	t_parser	*last_cmd;
	t_env		*env_list;
}	t_all;

// ==================== FUNCTION PROTOTYPES ====================

// ========== SHELL MAIN & INITIALIZATION ==========
int			main(int ac, char **av, char **env);
t_env		*initialize_shell(char **env);
void		shell_loop(t_env *env_list, char **env);
void		init_shell_variables(t_env **env_list);
int			process_command_line(char *line, t_env **env_list, char **env);

// ========== TOKENIZER & LEXER ==========
t_tokenizer	*tokenizer_init(char *input);
t_token		*create_token(t_token_type type, char *value);
t_token		*tokenize_input(char *input);
t_token		*tokenizer_get_next_token(t_tokenizer *tokenizer);
char		*extract_complex_word(t_tokenizer *tokenizer);
void		tokenizer_free(t_tokenizer *tokenizer);
void		lexer_advance(t_tokenizer *tokenizer);
char		lexer_peek(t_tokenizer *tokenizer);
int			ft_ispace(char c);
void		ft_skip_space(t_tokenizer *tokenizer);
int			check_redirection_syntax(char *input);
int			ft_tokenize_control(char *input);

// ========== PARSER ==========
t_parser	*parse_tokens(t_token *tokens, t_env *env_list);
void		init_parser_cmd(t_parser *cmd);
t_token		*ft_control_token(t_token *tokens, t_all *data);
int			check_token_syntax(t_token *tokens);

// ========== PARSER REDIRECTIONS ==========
void		add_redirection(t_parser *cmd, t_redir_type type, char *filename);
void		add_redirection_with_expansion(t_parser *cmd, t_redir_type type,
				char *filename, int no_expand);
void		process_redirection_expansion(t_token *tokens, t_parser *cmd,
				t_all *all, t_redir_type type);
void		ft_redir_heredoc(t_token *tokens, t_parser *cmd);

// ========== PARSER EXPANSION ==========
void		expand_argv_capacity(t_parser *cmd, int *argc);
int			is_empty_expansion(char *original_token, char *expanded);
int			should_split_expansion(char *original_token, char *expanded);
void		handle_split_expansion(t_parser *cmd, int *argc,
				char *original_token, char *expanded);
void		handle_normal_expansion(t_parser *cmd, int *argc,
				t_token *token, char *expanded);

// ========== PARSER UTILITIES ==========
int			has_quote_chars(char *delimiter);
char		*remove_quotes(char *delimiter);
char		**split_expanded_string(char *str);

// ========== EXECUTION ==========
int			execute_cmds(t_parser *cmd_list, char **env, t_env **env_list);
int			process_command(t_parser *cmd, t_exec_data *data, t_env **env_list);
void		execute_loop(t_parser *cmd_list, t_exec_data *data, t_env **env_list);
void		wait_pids(t_exec_data *data);
int			count_commands(t_parser *cmd);

// ========== EXECUTION PROCESS ==========
void		child_process_exec(t_parser *cmd, t_exec_data *data, t_env **env_list);
void		ft_exec_start(t_parser *cmd, t_exec_data *data, t_env **env_list);

// ========== EXECUTION UTILITIES ==========
void		close_all_fds_except_std(t_parser *cmd);
char		*find_executable(char *cmd);
int			is_builtin(t_parser *cmd);
int			ft_builtin_call(t_parser *cmd, t_exec_data *data, t_env **env_list);
void		ft_putendl_fd(char *str, int fd);
int			is_numeric_string(char *str);

// ========== EXECUTION REDIRECTIONS ==========
int			ft_redir_ctrl(t_parser *cmd);
int			setup_file_redirections(t_parser *cmd);

// ========== HEREDOC ==========
int			process_heredocs(t_parser *cmd, t_env *env_list);
char		*heredoc_readline(const char *prompt);
int			heredoc_append_line(t_heredoc_buffer *buf);
char		*readline_loop(t_heredoc_buffer *buf, const char *delimiter);
char		*read_single_heredoc_block(char *delimiter);
char		*read_heredoc_with_expand(char *delimiter, t_env *env_list);
int			ft_h_built_expand(t_redirection *current_redir,
				t_heredoc_data *data, t_env *env_list);

// ========== BUILTINS ==========
void		built_echo(t_parser *cmd);
int			built_cd(t_parser *cmd);
void		builtin_pwd(void);
void		builtin_exit(t_parser *cmd);
void		builtin_export(t_parser *cmd, t_env **env_list);
void		builtin_unset(t_parser *cmd, t_env **env_list);
void		builtin_env(t_env *env_list);

// ========== EXPANSION ==========
char		*expand_variables(char *str, t_env *env_list);
char		*expand_with_quotes(char *str, t_env *env_list);
char		*handle_dollar(char *str, int *pos, t_env *env_list);
char		*expand_heredoc_line(char *str, t_env *env_list);
int			is_valid_char(char c);
char		*push_char_res(char *res, char c, int *res_len, int *res_cap);
char		*append_string_res(char *result, char *str, int *res_len, int *res_cap);

// ========== ENVIRONMENT ==========
t_env		*init_env(char **env);
t_env		*find_env(t_env *env_list, const char *key);
char		*get_env_value(t_env *env_list, char *key);
void		set_env_value(t_env **env_list, char *key, char *value);
void		unset_env_value(t_env **env_list, char *key);
char		**env_list_to_array(t_env *env_list);
void		free_env_list(t_env *env_list);

// ========== EXPORT ==========
t_export	**get_export_list(void);
t_export	*find_export(t_export *export_list, const char *key);
void		set_export_value(t_export **export_list, const char *key,
				const char *value);
void		unset_export_value(t_export **export_list, const char *key);
char		*find_export_value(char *key);
t_export	*init_export_from_env(t_env *env_list);
void		free_export_list(t_export *export_list);

// ========== SIGNAL HANDLING ==========
void		setup_interactive_signals(void);
void		setup_parent_execution_signals(void);
void		setup_child_signals(void);
void		heredoc_signals(void);
void		handle_sigint(int signum);
void		handle_sigint_heredoc(int signum);
int			calculate_exit_status(int status);

// ========== GARBAGE COLLECTOR ==========
t_gb		**get_gb_list(void);
t_gb		**get_env_gb_list(void);
void		*gb_malloc(size_t size);
void		gb_free_all(void);
void		gc_free(void *ptr);
void		*env_gb_malloc(size_t size);
void		env_gb_free_all(void);
void		env_gc_free(void *ptr);
void		gc_free_from_list(t_gb **gb_list, void *ptr);
t_gb		*ft_lstnew(void *data);
void		gb_lstadd_front(t_gb **lst, t_gb *new_node);
void		*gc_safe(void *ptr);
void		*env_gc_safe(void *ptr);
void		fail_exit(void);

// ========== EXIT STATUS ==========
int			get_last_exit_status(void);
void		set_last_exit_status(int status);

// ========== UTILITIES ==========
int			ft_strlen(const char *s);
char		*ft_strdup(char *str);
char		*ft_strndup(const char *str, size_t n);
int			ft_strcmp(const char *s1, const char *s2);
char		*ft_strchr(const char *s, int c);
char		*ft_strcpy(char *dest, const char *src);
char		*ft_strcat(char *dest, const char *src);
char		*ft_strncpy(char *dest, const char *src, size_t n);
size_t		ft_strlcpy(char *dst, const char *src, size_t size);
char		*ft_itoa(int n);
int			ft_atoi(char *str);
void		ft_memcpy(char *s1, const char *s2, int len);
int			is_quote(char c);
int	redir_in(t_redirection *redir);
int	redir_out(t_redirection *redir);
int	redir_append(t_redirection *redir);

#endif