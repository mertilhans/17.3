/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:35 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/31 18:06:51 by merilhan         ###   ########.fr       */
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
# include "env/env.h"
# include "gb/gb.h"
# include "lexer/shell.h"

// Sadece exit status global kalabilir (signal handler için gerekli)
extern int g_last_exit_status;

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
    struct s_redirection *next;     // Bir sonraki redirection
} t_redirection;

// Komut yapısı (t_parser olarak adlandırılmış)
typedef struct s_parser
{
    char                **argv;         // Komut ve argümanlar
    int                 argv_cap;       // argv dizisinin kapasitesi
    t_redirection       *redirs;        // Redirection'lar için bağlı liste
    struct s_parser *next;          // Pipe için
    int                 parse_error;    // Ayrıştırma sırasında hata olup olmadığını belirtir (opsiyonel)
    int                 heredoc_fd;
} t_parser;

// --- PARSER FONKSİYON PROTOTİPLERİ ---
t_parser *parse_tokens(t_token *tokens);
void print_cmds(t_parser *cmd_list);
void ft_add_redirection(t_parser *cmd, char *filename, t_redir_type type);
void ft_clean_cmd_node(t_parser *cmd);

// Tokenizer fonksiyon prototipleri
t_tokenizer *tokenizer_init(char *input);
void        tokenizer_free(t_tokenizer *tokenizer);
t_token     *tokenizer_get_next_token(t_tokenizer *tokenizer);
t_token     *tokenize_input(char *input);
void        token_free(t_token *token);
void        token_list_free(t_token *tokens);
void        print_tokens(t_token *tokens);

// Yardımcı fonksiyonlar
int         is_whitespace(char c);
int         is_special_char(char c);
int         is_quote(char c);
char        *extract_complex_word(t_tokenizer *tokenizer);

t_token *tokenize_input(char *input);
t_token *token_input_2(t_tokenizer *tokenizer, t_token *tokens, t_token *current);
t_token *tokenizer_get_next_token(t_tokenizer *tokenizer);
t_token *tokenizer_get_next_token_2(t_tokenizer *tokenizer);
t_tokenizer *tokenizer_init(char *input);
t_token *create_token(t_token_type type, char *value);

void    ft_skip_space(t_tokenizer *tokenizer);
char lexer_peek(t_tokenizer *tokenizer);
int ft_ispace(char c);
void    lexer_advance(t_tokenizer *tokenizer);
void    tokenizer_free(t_tokenizer *tokenizer);
int is_quote(char c);
size_t ft_strlen(const char *s);
char *ft_strdup(char *str);

void print(t_parser *c);
void execute_cmds(t_parser *cmd_list, char **env, t_env **env_list);
void free_parser_list(t_parser *cmd_list);
int count_commands(t_parser *cmd);

void expand_parser_list(t_parser *cmd_list, t_env *env_list);

// Builtin fonksiyonları - artık env_list parametresi alıyor
int     built_echo(t_parser *cmd);
int     built_cd(t_parser *cmd, t_env **env_list);
int     builtin_pwd(void);
int     builtin_exit(t_parser *cmd);
int     builtin_export(t_parser *cmd, t_env **env_list);
int     builtin_unset(t_parser *cmd, t_env **env_list);
int     builtin_env(t_parser *cmd, t_env *env_list);

void expand_parser_list(t_parser *cmd_list, t_env *env_list);
char    *ft_substr(char const *s, unsigned int start, size_t len);
size_t  ft_strlcpy(char *dest, const char *src, size_t dsize);
char    *ft_itoa(int n);
t_env *find_env(t_env *env_list, const char *key);

int     is_valid_char(char c);
char *expand_variables(char *str, t_env *env_list);
char *expander_loop(char *str, t_env *env_list, char *res, int *res_len);
char *handle_dollar(char *str, int *pos, t_env *env_list);
char *handle_simple(const char *str, int *i, t_env *env_list);
char *expand_with_quotes(char *str, t_env *env);

char    *push_char_res(char *res, char c, int *res_len, int *res_cap);
char    *push_res(char *res, const char *to_add, int *res_len, int *res_cap);
char    *ft_strchr(const char *s, int c);
int    ft_strcmp(const char *s1, const char *s2);
char *ft_strcpy(char *dest, const char *src);
char *ft_strcat(char *dest, const char *src);

// Memory management
void *ft_memcpy(void *dst, const void *src, size_t n);

// Environment utilities
char    *get_env_value(t_env *env_list, char *key);
t_env   *init_env(char **env);
void    set_env_value(t_env **env_list, char *key, char *value);
void    unset_env_value(t_env **env_list, char *key);

//signal
void handle_sigint(int signum);
void setup_interactive_signals(void);
void setup_child_signals(void);
void check_and_handle_signal(void);

char *find_executable(char *cmd);
char *remove_quotes_exec(char *str);
#endif