/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 18:45:45 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 18:51:15 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <unistd.h>

typedef struct s_heredoc_data
{
	int						last_heredoc_fd;
	size_t					heredoc_len;
	char					*heredoc_content;
}							t_heredoc_data;

typedef struct s_exec_data
{
	int						in_fd;
	int						pipefd[2];
	int						*pids;
	int						i;
	int						pids_size;
	char					**env;
	int						original_stdin;
	int						original_stdout;
}							t_exec_data;

typedef struct s_heredoc_buffer
{
	char					*content;
	char					*line;
	char					*line_with_nl;
	char					*new_content;
	size_t					delimiter_len;
}							t_heredoc_buffer;

typedef struct s_token_data
{
	char					*word;
	char					in_quote;
	int						len;
	int						capacity;
}							t_token_data;

typedef struct s_gb
{
	void					*data;
	struct s_gb				*next;
}							t_gb;

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
}							t_token_type;

typedef struct s_token
{
	t_token_type			type;
	char					*value;
	struct s_token			*next;
}							t_token;

typedef struct s_env
{
	char					*key;
	char					*value;
	struct s_env			*next;
}							t_env;

typedef struct s_expander_state
{
	int						*i;
	char					*quote_char;
	t_env					*env_list;
}							t_expander_state;

typedef struct s_export
{
	char					*key;
	char					*value;
	struct s_export			*next;
}							t_export;

typedef struct s_tokenizer
{
	char					*input;
	int						pos;
	int						len;
	char					current;
}							t_tokenizer;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}							t_redir_type;

typedef struct s_redirection
{
	char					*filename;
	t_redir_type			type;
	int						no_expand;
	struct s_redirection	*next;
}							t_redirection;

typedef struct s_parser
{
	int						fd_i;
	int						fd_o;
	char					**argv;
	int						argv_cap;
	t_redirection			*redirs;
	struct s_parser			*next;
	int						heredoc_fd;
}							t_parser;

typedef struct s_all
{
	t_parser				*cmd_list;
	t_parser				*last_cmd;
	t_env					*env_list;
}							t_all;

typedef struct s_quote_data
{
	char					*result;
	int						res_len;
	int						res_cap;
}							t_quote_data;

typedef struct s_heredoc_state
{
	char					*line;
	size_t					len;
	int						at_line_start;
}							t_heredoc_state;

t_env						*initialize_shell(char **env);
void						shell_loop(t_env *env_list, char **env);
int							process_command_line(char *line, t_env **env_list,
								char **env);

t_tokenizer					*tokenizer_init(char *input);
t_token						*create_token(t_token_type type, char *value);
t_token						*tokenize_input(char *input);
t_token						*tokenizer_get_next_token(t_tokenizer *tokenizer);
void						tokenizer_free(t_tokenizer *tokenizer);
void						lexer_advance(t_tokenizer *tokenizer);
char						lexer_peek(t_tokenizer *tokenizer);
void						ft_skip_space(t_tokenizer *tokenizer);
char						*extract_complex_word(t_tokenizer *tokenizer);

t_parser					*parse_tokens(t_token *tokens, t_env *env_list);
void						init_parser_cmd(t_parser *cmd);
void						finalize_cmd(t_parser *cmd, int argc, t_all *data);
void						expand_argv_capacity(t_parser *cmd, int *argc);
void						add_redirection_with_expansion(t_parser *cmd,
								t_redir_type type, char *filename,
								int no_expand);
void						init_redirection(t_redirection *redir,
								t_redir_type type, char *filename,
								int no_expand);
void						ft_temp_next(t_redirection *redir, t_parser *cmd);
int							has_quote_chars(char *str);
char						*remove_quotes(char *str);
void						add_split_args(t_parser *cmd, int *argc,
								char **split_args);
void						cleanup_split_args(char **split_args);
int							is_empty_expansion(char *original_token,
								char *expanded);
int							should_split_expansion(char *original_token,
								char *expanded);
void						redir_control_state(t_token *token, t_parser *cmd,
								t_all *all, int result);
void						ft_redir_heredoc(t_token *token, t_parser *cmd);
void						ft_loop(t_token **tokens, t_parser *cmd, int *argc,
								t_all *data);
int							ft_syn(t_token *tokens);

int							execute_cmds(t_parser *cmd_list, char **env,
								t_env **env_list);
int							is_builtin(t_parser *cmd);
int							ft_builtin_call(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
int							count_commands(t_parser *cmd);
void						exe_loop(t_parser *cmd_list, t_exec_data *data,
								t_env **env_list);
void						execute_loop(t_parser *cmd_list, t_exec_data *data,
								t_env **env_list);
void						wait_pids(t_exec_data *data);
void						fd_not_stdin(t_exec_data *data);
void						next_cmd_and_fd(t_exec_data *data);
void						ft_free_and_close(t_exec_data *data, t_parser *cmd);
void						ft_exec_start(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
void						ft_exec_ctrl(t_parser *cmd, t_exec_data *data,
								char **exec_path, struct stat *path_stat);
char						*find_executable(char *cmd, t_env *env_list);
void						ft_not_executable(t_parser *cmd, t_exec_data *data,
								char *exec_path);
void						ft_permission_denied(t_parser *cmd,
								t_exec_data *data);
void						ft_directory(t_parser *cmd, t_exec_data *data);
void						ft_not_directory(t_parser *cmd, t_exec_data *data);
int							data_init(t_exec_data *data, t_parser *cmd_list,
								char **env, int *pid_len);
int							heredoc_handle(t_parser *cmd_list,
								t_exec_data *data, t_env *env_list);
int							heredoc_handle_init(t_parser *cmd_list);
int							execute_cmds_2(t_parser *cmd_list,
								t_exec_data *data, t_env **env_list,
								int *pid_len);
int							ft_data_pids(t_parser *cmd_list, t_exec_data *data,
								t_env **env_list);
void						finish_fd(t_parser *cmd_list, t_exec_data *data);
int							process_command(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
int							setup_and_fork(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
int							n_next_or_built(t_parser *cmd_list,
								t_exec_data *data, t_env **env_list);
int							setup_pipe_if_needed(t_parser *cmd,
								t_exec_data *data);
int							fork_pipe_exec(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
void						child_process_exec(t_parser *cmd, t_exec_data *data,
								t_env **env_list);
int							is_exec_other(char *cmd);
char						*search_in_path(char *cmd, char *path);
char						*check_cmd_in_dir(const char *dir_start, int len,
								char *cmd);
char						*copy_dir(const char *dir_start, int len);
char						*join_path(char *dir, char *file);
int							ft_builtin_call_2(t_parser *cmd, t_env **env_list);
int							ft_builtin_call_3(t_parser *cmd, t_env **env_list);

void						built_echo(t_parser *cmd);
int							built_cd(t_parser *cmd, t_env **env_list,
								t_export **export_list);
void						builtin_pwd(void);
void						builtin_exit(t_parser *cmd);
void						builtin_export(t_parser *cmd, t_env **env_list);
void						builtin_unset(t_parser *cmd, t_env **env_list);
void						builtin_env(t_env *env_list);

t_env						*init_env(char **env);
t_env						*find_env(t_env *env_list, const char *key);
char						*get_env_value(t_env *env_list, char *key);
void						set_env_value(t_env **env_list, char *key,
								char *value);
void						unset_env_value(t_env **env_list, char *key);
char						**env_list_to_array(t_env *env_list);
char						**create_sorted_env_array(t_env *env_list);

t_export					**get_export_list(void);
t_export					*find_export(t_export *export_list,
								const char *key);
char						*find_export_value(char *key);
void						set_export_value(t_export **export_list,
								const char *key, const char *value);
void						unset_export_value(t_export **export_list,
								const char *key);
t_export					*init_export_from_env(t_env *env_list);
void						export_process_keyvalue(t_parser *cmd, int *i,
								t_env **env_list);
void						export_print_all(t_export **export_list);
char						*export_build_value(t_parser *cmd, int *i,
								char *value);

char						*expand_with_quotes(char *str, t_env *env_list);
char						*expand_heredoc_line(char *str, t_env *env_list);
char						*handle_dollar(char *str, int *pos,
								t_env *env_list);
char						**split_expanded_string(char *str);
char						*handle_special_vars(char c);
void						process_quote_char(char *str, int *i,
								char *quote_char);
char						*push_char_res(char *result, char c, int *res_len,
								int *res_cap);
char						*append_string_res(char *result, char *to_append,
								int *res_len, int *res_cap);
t_quote_data				*init_quote_data(char *str);
int							is_valid_char(char c);
char						*realloc_result(char *result, int new_size);

int							process_heredocs(t_parser *cmd, t_env *env_list);
char						*heredoc_readline(const char *prompt);
char						*read_single_block(char *delimiter);
char						*read_heredoc_expand(char *delimiter,
								t_env *env_list);
char						*expand_heredoc_line_impl(char *line,
								t_env *env_list);
int							h_loop(t_parser *cmd, t_heredoc_data *h_data,
								t_env *env_list);
int							ft_h_built_expand(t_redirection *current_redir,
								t_heredoc_data *data, t_env *env_list);
void						ft_h_buffer_init(t_heredoc_buffer *buf,
								char *delimiter);
char						*readline_loop(t_heredoc_buffer *buf,
								const char *delimiter);
char						*readline_loop_expand(t_heredoc_buffer *buf,
								const char *delimiter, t_env *env_list);
int							process_null_line(t_heredoc_buffer *buf,
								const char *delimiter, char **result);
int							check_delimiter(t_heredoc_buffer *buf,
								const char *delimiter);
int							process_line(t_heredoc_buffer *buf, t_env *env_list,
								int expand);
int							heredoc_append_expanded(t_heredoc_buffer *buf,
								t_env *env_list);
int							heredoc_append_line(t_heredoc_buffer *buf);
void						not_her_app_exp(t_heredoc_buffer *buf);
int							process_read_char(ssize_t bytes, char ch,
								t_heredoc_state *st);
int							append_char(t_heredoc_state *st, char ch);
char						*finalize_line(t_heredoc_state *st);

void						setup_interactive_signals(void);
void						setup_parent_execution_signals(void);
void						setup_child_signals(void);
void						heredoc_signals(void);

void						*gb_malloc(size_t size);
void						gb_free_all(void);
void						gc_free(void *ptr);
void						*env_gb_malloc(size_t size);
void						env_gb_free_all(void);
void						env_gc_free(void *ptr);

t_gb						**get_gb_list(void);
void						*gc_safe(void *ptr);
void						fail_exit(void);
t_gb						*ft_lstnew(void *data);
void						gb_lstadd_front(t_gb **lst, t_gb *new_node);
void						gc_free_from_list(t_gb **gb_list, void *ptr);

int							get_last_exit_status(void);
void						set_last_exit_status(int status);
int							calculate_exit_status(int status);

int							ft_strlen(const char *s);
int							ft_strcmp(const char *s1, const char *s2);
int							ft_atoi(char *str);
char						*ft_strdup(char *str);
char						*ft_strndup(const char *str, size_t n);
char						*ft_strchr(const char *s, int c);
char						*ft_strcpy(char *dest, const char *src);
char						*ft_strcat(char *dest, const char *src);
char						*ft_itoa(int n);
void						ft_memcpy(char *s1, const char *s2, int len);
int							ft_ispace(char c);
int							is_valid_identifier(const char *identifier);
void						ft_putendl_fd(char *s, int fd);
void						ft_in_or_out(t_exec_data *data);

void						close_heredoc_fd(t_parser *cmd);
int							ft_redir_ctrl(t_parser *cmd);
int							redir_in(t_redirection *redir);
int							ft_redir_in_or_out(t_redirection *redir);
int							ft_heredoc(t_parser *cmd);
int							heredoc_fd_error(t_parser *cmd_list,
								t_exec_data *data, t_parser *current_cmd);
void						close_fd(t_exec_data *data);

t_parser					*parse_line(char *line, t_env *env_list);
void						cleanup_env_array(char **env_array);
void						init_shell_variables(t_env **env_list);
t_token						*tokenizer_get_next_token_2(t_tokenizer *tokenizer);
int							ft_tokenize_control(char *input);
char						*lexer_append_char(char c, t_token_data *data);
int							is_special_char(char c);
void						ft_token_data_init(t_token_data *data);
void						ft_skip_space(t_tokenizer *tokenizer);
char						lexer_peek(t_tokenizer *tokenizer);
void						lexer_advance(t_tokenizer *tokenizer);
void						tokenizer_free(t_tokenizer *tokenizer);
int							ft_ispace(char c);
void						swap_export_nodes(t_export *a, t_export *b);
void						init_expander_state(t_expander_state *state,
								int *i_ptr, char *q_ptr, t_env *env_list);

#endif
