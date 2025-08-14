#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <errno.h>
# include <ctype.h>
# include <stdint.h>
# include <readline/readline.h>
# include <readline/history.h>

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


int				main(int ac, char **av, char **env);
t_env			*initialize_shell(char **env);
void			shell_loop(t_env *env_list, char **env);
int				process_command_line(char *line, t_env **env_list, char **env);
void			init_shell_variables(t_env **env_list);
void			cleanup_env_array(char **env_array);


t_token			*tokenize_input(char *input);
t_tokenizer		*tokenizer_init(char *input);
t_token			*create_token(t_token_type type, char *value);
void			tokenizer_free(t_tokenizer *tokenizer);
t_token			*tokenizer_get_next_token(t_tokenizer *tokenizer);
char			*extract_complex_word(t_tokenizer *tokenizer);
void			lexer_advance(t_tokenizer *tokenizer);
char			lexer_peek(t_tokenizer *tokenizer);
void			ft_skip_space(t_tokenizer *tokenizer);
int				ft_ispace(char c);
int				is_special_char(char c);
int				ft_tokenize_control(char *input);

t_parser		*parse_tokens(t_token *tokens, t_env *env_list);
t_parser		*parse_token_list(t_token *tokens, t_all *all);
void			add_redirection(t_parser *cmd, t_redir_type type, char *f);
void			add_redirection_with_expansion(t_parser *cmd, t_redir_type t,
					char *f, int no_expand);
void			init_parser_cmd(t_parser *cmd);
void			expand_argv_capacity(t_parser *cmd, int *argc);
int				should_split_expansion(char *original_token, char *expanded);
void			handle_split_expansion(t_parser *cmd, int *argc,
					char *original_token, char *expanded);
void			handle_normal_expansion(t_parser *cmd, int *argc,
					t_token *token, char *expanded);
int				has_quote_chars(char *delimiter);
char			*remove_quotes(char *delimiter);				

int				execute_cmds(t_parser *cmd_list, char **env, t_env **env_list);
int				process_command(t_parser *cmd, t_exec_data *data,
					t_env **env_list);
void			child_process_exec(t_parser *cmd, t_exec_data *data,
					t_env **env_list);
int				setup_file_redirections(t_parser *cmd);
char			*find_executable(char *cmd);
void			close_all_fds_except_std(t_parser *cmd);
int				calculate_exit_status(int status);

void			built_echo(t_parser *cmd);
int				built_cd(t_parser *cmd);
void			builtin_pwd(void);
void			builtin_exit(t_parser *cmd);
void			builtin_env(t_env *env_list);
void			builtin_export(t_parser *cmd, t_env **env_list);
void			builtin_unset(t_parser *cmd, t_env **env_list);
int				is_builtin(t_parser *cmd);
int				ft_builtin_call(t_parser *cmd, t_exec_data *data,
					t_env **env_list);

t_env			*init_env(char **env);
t_env			*find_env(t_env *env_list, const char *key);
char			*get_env_value(t_env *env_list, char *key);
void			set_env_value(t_env **env_list, char *key, char *value);
void			unset_env_value(t_env **env_list, char *key);
char			**env_list_to_array(t_env *env_list);

t_export		**get_export_list(void);
t_export		*find_export(t_export *export_list, const char *key);
char			*find_export_value(char *key);
void			set_export_value(t_export **export_list, const char *key,
					const char *value);
void			unset_export_value(t_export **export_list, const char *key);
t_export		*init_export_from_env(t_env *env_list);
char			*export_build_value(t_parser *cmd, int *i, char *value); 

char			*expand_with_quotes(char *str, t_env *env_list);
char			*expand_variables(char *str, t_env *env_list);
char			*expand_heredoc_line(char *str, t_env *env_list);
char			*handle_dollar(char *str, int *i, t_env *env_list);
char			**split_expanded_string(char *str);
int				is_empty_expansion(char *original_token, char *expanded);

int				process_heredocs(t_parser *cmd, t_env *env_list);
char			*heredoc_readline(const char *prompt);
char			*read_single_heredoc_block(char *delimiter);
char			*read_heredoc_with_expand(char *delimiter, t_env *env_list);
int				heredoc_append_line(t_heredoc_buffer *buf);

void			setup_interactive_signals(void);
void			setup_parent_execution_signals(void);
void			setup_child_signals(void);
void			heredoc_signals(void);
void			handle_sigint(int signum);
void			handle_sigint_heredoc(int signum);

t_gb			**get_gb_list(void);
t_gb			**get_env_gb_list(void);
void			*gb_malloc(size_t size);
void			gc_free(void *ptr);
void			gb_free_all(void);
void			env_gb_free_all(void);
void			*env_gb_malloc(size_t size);
void			env_gc_free(void *ptr);
void			fail_exit(void);

int				get_last_exit_status(void);
void			set_last_exit_status(int status);

int				ft_strlen(char *s);
char			*ft_strdup(char *str);
char			*ft_strndup(const char *str, size_t n);
char			*ft_strchr(const char *s, int c);
int				ft_strcmp(const char *s1, const char *s2);
char			*ft_strcpy(char *dest, const char *src);
char			*ft_strcat(char *dest, const char *src);
void			ft_memcpy(char *s1, const char *s2, int len);
int				ft_atoi(char *str);
char			*ft_itoa(int n);
void			ft_putendl_fd(char *str, int fd);
void process_redirection_expansion(t_token *tokens, t_parser *cmd, t_all *all, t_redir_type type);
int count_commands(t_parser *cmd);
void execute_pipeline(t_parser *cmd_list, t_exec_data *data, t_env **env_list);
void finish_execution(t_parser *cmd_list, t_exec_data *data);
void restore_fds(t_exec_data *data);
char *readline_loop_expand(t_heredoc_buffer *buf, const char *delimiter, t_env *env_list);
void set_export_value_existing(t_export *existing, const char *value);
void set_export_value_new(t_export **export_list, const char *key, const char *value);
void *gc_safe(void *ptr);
int heredoc_append_expanded(t_heredoc_buffer *buf, t_env *env_list);
void ft_directory(t_parser *cmd, t_exec_data *data);
void ft_permission_denied(t_parser *cmd, t_exec_data *data);
void ft_not_directory(t_parser *cmd, t_exec_data *data);
void ft_not_executable(t_parser *cmd, t_exec_data *data, char *exec_path);


 
t_gb *ft_lstnew(void *data);
void gb_lstadd_front(t_gb **lst, t_gb *new_node);
void gc_free_from_list(t_gb **gb_list, void *ptr);
 
#endif

// --- Garbage collector helpers (sadece kullanılanlar) ---