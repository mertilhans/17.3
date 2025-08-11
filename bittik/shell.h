/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:35 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/11 06:44:19 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
#include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
#include <sys/wait.h>
#include <signal.h>

// Garbage Collector structure
typedef struct s_heredoc_data
{
    int last_heredoc_fd;
    size_t heredoc_len;
    char *heredoc_content;
    
}t_heredoc_data;

typedef struct s_exec_data
{
    int in_fd;
    int pipefd[2];
    int *pids;
    int i;
    int pids_size;
    char **env;
    int original_stdin;
    int original_stdout;
} t_exec_data;

typedef struct s_heredoc
{
    char *content;
    size_t len;
} t_heredoc;

typedef struct s_heredoc_buffer
{
    char *content;
    char *line;
    char *line_with_nl;
    char *new_content;
    size_t delimiter_len;
} t_heredoc_buffer;

typedef struct s_reader
{
    char *buf;
    size_t size;
    size_t pos;
} t_reader;

typedef struct  s_token_data
{
	char *word;
	char in_quote;
	int len;
	int capacity;
	
}t_token_data;

typedef struct s_gb
{
	void *data;
    struct s_gb *next;
} t_gb;

// Token tipleri
typedef enum e_token_type
{
	TOKEN_WORD,          // Normal kelimeler
    TOKEN_PIPE,          // |
    TOKEN_REDIR_IN,      // <
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_HEREDOC,       // <<
    TOKEN_EOF,           // Dosya sonu
    TOKEN_ERROR          // Hata
} t_token_type;

// Token yapısı
typedef struct s_token
{
	t_token_type    type;
    char            *value;
    struct s_token  *next;
} t_token;

// Environment variable structure
typedef struct s_env
{
	char *key;
    char *value;
    struct s_env *next;
} t_env;

typedef struct s_export
{
	char *key;
    char *value;  // NULL olabilir (sadece export a durumu için)
    struct s_export *next;
} t_export;

// Tokenizer yapısı
typedef struct s_tokenizer
{
	char    *input;      // Giriş metni
    int     pos;         // Mevcut pozisyon
    int     len;         // Toplam uzunluk
    char    current;     // Mevcut karakter
} t_tokenizer;

// Redirection türlerini tanımlayan enum
typedef enum e_redir_type
{
	REDIR_IN,       // <
    REDIR_OUT,      // >
    REDIR_APPEND,   // >>
    REDIR_HEREDOC   // <<
} t_redir_type;

// Her bir redirection için bilgi tutan yapı
typedef struct s_redirection
{
	char                *filename; // Yönlendirilecek dosya adı veya heredoc delimiter
    t_redir_type        type;      // Redirection tipi
    int                 no_expand; // Heredoc expansion flag (1 = no expansion, 0 = expand)
    struct s_redirection *next;     // Bir sonraki redirection
} t_redirection;

// Komut yapısı (t_parser olarak adlandırılmış)
typedef struct s_parser
{
    int fd_i;
    int fd_o;
	char                **argv;
    int                 argv_cap;
    t_redirection       *redirs;
    struct s_parser     *next;
    int                 parse_error;
    int                 heredoc_fd; // Her komutun kendi heredoc girdisi için
} t_parser;

typedef struct s_all
{
    t_parser *cmd_list;
    t_parser *last_cmd;
    t_env  *env_list;
}t_all;

// --- PARSER FONKSİYON PROTOTİPLERİ ---
t_parser *parse_tokens(t_token *tokens, t_env *env_list);

// NEW PARSER FUNCTIONS
void init_redirection(t_redirection *new_redir, t_redir_type type, char *filename, int no_expand);
void process_redirection_expansion(t_token *tokens, t_parser *cmd, t_all *all, t_redir_type type);
int has_quote_chars(char *delimiter);
char *remove_quotes(char *delimiter);
void expand_argv_capacity(t_parser *cmd, int *argc);
int is_empty_expansion(char *original_token, char *expanded);
int should_split_expansion(char *original_token, char *expanded);
void add_split_args(t_parser *cmd, int *argc, char **split_args);
void cleanup_split_args(char **split_args);
void handle_split_expansion(t_parser *cmd, int *argc, char *original_token, char *expanded);
void handle_normal_expansion(t_parser *cmd, int *argc, t_token *token, char *expanded);
void init_parser_cmd(t_parser *cmd);
void finalize_cmd(t_parser *cmd, int argc, t_all *data);
void handle_pipe_errors(t_token **tokens);
int check_parse_errors(t_all *all);

void ft_add_redirection(t_parser *cmd, char *filename, t_redir_type type);
void add_redirection_with_expansion(t_parser *cmd, t_redir_type type, char *filename, int no_expand);
void ft_clean_cmd_node(t_parser *cmd);

// --- DİĞER FONKSİYON PROTOTİPLERİ ---
t_tokenizer *tokenizer_init(char *input);
void        tokenizer_free(t_tokenizer *tokenizer);
t_token     *tokenizer_get_next_token(t_tokenizer *tokenizer);
t_token     *tokenize_input(char *input);
void        token_free(t_token *token);
void        token_list_free(t_token *tokens);

// Yardımcı fonksiyonlar
int         is_whitespace(char c);
int         is_special_char(char c);
int         is_quote(char c);
char        *extract_complex_word(t_tokenizer *tokenizer);

t_token *tokenize_input(char *input);
t_token *token_input_2(t_tokenizer *tokenizer, t_token *tokens, t_token *current);
t_token *tokenizer_get_next_token_2(t_tokenizer *tokenizer);
t_tokenizer *tokenizer_init(char *input);
t_token *create_token(t_token_type type, char *value);

void    ft_skip_space(t_tokenizer *tokenizer);
char lexer_peek(t_tokenizer *tokenizer);
int ft_ispace(char c);
void    lexer_advance(t_tokenizer *tokenizer);
void    tokenizer_free(t_tokenizer *tokenizer);
void ft_memcpy(char *s1,const char *s2, int len);
int is_quote(char c);
int ft_strlen(char *s);
char *ft_strdup(char *str);
char *ft_strndup(const char *str, size_t n);
char *ft_strchr(const char *s, int c);
char *ft_strncpy(char *dest, const char *src, size_t n);
size_t ft_strlcpy(char *dst, const char *src, size_t size);
char *ft_strcpy(char *dest, const char *src);
char *ft_strcat(char *dest, const char *src);
char *ft_itoa(int n);

int execute_cmds(t_parser *cmd_list, char **env, t_env **env_list);
void free_parser_list(t_parser *cmd_list);
int count_commands(t_parser *cmd);
// DÜZELTİLDİ: Tanıma uyması için char* yerine const char* yapıldı
int ft_strcmp(const char *s1, const char *s2);

int ft_atoi(char *str);

void     built_echo(t_parser *cmd);
int      built_cd(t_parser *cmd);
void     builtin_pwd(void);
void builtin_exit(t_parser *cmd);
void builtin_export(t_parser *cmd, t_env **env_list);
void builtin_unset(t_parser *cmd, t_env **env_list);
void builtin_env(t_env *env_list);

// Builtin helper functions
void export_print_all(t_export **export_list);
char *export_build_value(t_parser *cmd, int *i, char *value);
void export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list);

// Export list functions
t_export *find_export(t_export *export_list, const char *key);
void set_export_value(t_export **export_list, const char *key, const char *value);
void unset_export_value(t_export **export_list, const char *key);
void free_export_list(t_export *export_list);
t_export *init_export_from_env(t_env *env_list);
t_export **get_export_list(void);  // Helper for accessing export list

// Shell initialization functions
void init_shell_variables(t_env **env_list);

// Signal handler functions
void handle_sigint(int signum);
void handle_sigint_heredoc(int signum);
void heredoc_signals(void);
void setup_interactive_signals(void);
void setup_parent_execution_signals(void);
void setup_child_signals(void);
int calculate_exit_status(int status);

// Garbage Collector functions
t_gb *ft_lstnew(void *data);
void gb_lstadd_front(t_gb **lst, t_gb *new_node);
void *gc_safe(void *ptr);
void *gb_malloc(size_t size);
void gb_free_all(void);
void gc_free(void *ptr);

// Environment-specific garbage collector (separate from main GC)
void *env_gc_safe(void *ptr);
void *env_gb_malloc(size_t size);
void env_gb_free_all(void);
void env_gc_free(void *ptr);

void	cleanup_env_array(char **env_array);

// Garbage Collector functions
t_gb **get_gb_list(void);
t_gb **get_env_gb_list(void);
t_gb *ft_lstnew(void *data);
void gb_lstadd_front(t_gb **lst, t_gb *new_node);
void fail_exit(void);
void *gc_safe(void *ptr);
void *gb_malloc(size_t size);
void gb_free_all(void);
void gc_free_from_list(t_gb **gb_list, void *ptr);
void *env_gc_safe(void *ptr);
void *env_gb_malloc(size_t size);
void env_gb_free_all(void);

// Environment functions
t_env *find_env(t_env *env_list, const char *key);
t_env *init_env(char **env);
char *get_env_value(t_env *env_list, char *key);
void set_env_value(t_env **env_list, char *key, char *value);
void unset_env_value(t_env **env_list, char *key);
void free_env_list(t_env *env_list);
char **env_list_to_array(t_env *env_list);
char *find_export_value(char *key);
void gc_free(void *ptr);

// Variable expansion functions
int is_valid_char(char c);
char *expand_variables(char *str, t_env *env_list);
char *expand_with_quotes(char *str, t_env *env_list);
char *handle_dollar(char *str, int *pos, t_env *env_list);
char *handle_simple(const char *str, int *i, t_env *env_list);
char *expand_heredoc_line(char *str, t_env *env_list);

char *ft_itoa(int n);
char *push_char_res(char *res, char c, int *res_len, int *res_cap);
char **split_expanded_string(char *str);

// Heredoc with expansion functions
char *read_heredoc_with_expand(char *delimiter, t_env *env_list);
int ft_h_built_expand(t_redirection *current_redir, t_heredoc_data *data, t_env *env_list);
int process_heredocs(t_parser *cmd, t_env *env_list);
int heredoc_append_line(t_heredoc_buffer *buf);
void not_her_app_exp(t_heredoc_buffer *buf);

void close_all_fds_except_std(t_parser *cmd);
void tt(t_exec_data *data);
int get_last_exit_status(void);
void set_last_exit_status(int status);

int process_command_line(char *line, t_env **env_list, char **env);
int process_command(t_parser *cmd, t_exec_data *data, t_env **env_list);
char *heredoc_readline(const char *prompt);

#endif