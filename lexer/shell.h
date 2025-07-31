/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:35 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/25 17:57:28 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>


typedef enum e_token_type
{
    TOKEN_WORD,          // Normal kelimeler
    TOKEN_PIPE,          // |
    TOKEN_REDIR_IN,      // <<
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_HEREDOC,       // <<
    TOKEN_AND,           // &&
    TOKEN_OR,            // ||
    TOKEN_SEMICOLON,     // ;
    TOKEN_LPAREN,        // (
        TOKEN_RPAREN,        // )
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
    
    // Fonksiyon prototipleri
    t_tokenizer *tokenizer_init(char *input);
    void        tokenizer_free(t_tokenizer *tokenizer);
    t_token     *tokenizer_get_next_token(t_tokenizer *tokenizer);
    t_token     *tokenize_input(char *input);
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
  

    void	ft_skip_space(t_tokenizer *tokenizer);
    char lexer_peek(t_tokenizer *tokenizer);
    int	ft_ispace(char c);
void	lexer_advance(t_tokenizer *tokenizer);
void	tokenizer_free(t_tokenizer *tokenizer);
int is_quote(char c);
char *ft_strdup(char *str);

    #endif