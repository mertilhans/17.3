/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:35:51 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/07 05:59:46 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "../gb/gb.h"

t_token *tokenizer_get_next_token_2(t_tokenizer *tokenizer)
{
    if (tokenizer->current == '\0')
        return create_token(TOKEN_EOF, NULL);
    if (tokenizer->current == '|')
    {
        if (lexer_peek(tokenizer) == '|')
        {
            lexer_advance(tokenizer);
            lexer_advance(tokenizer);
            return create_token(TOKEN_OR, "||");
        }
        lexer_advance(tokenizer);
        return create_token(TOKEN_PIPE, "|");
    }
    if (tokenizer->current == '<')
    {
        if (lexer_peek(tokenizer) == '<')
        {
            lexer_advance(tokenizer);
            lexer_advance(tokenizer);
            return create_token(TOKEN_HEREDOC, "<<");
        }
        lexer_advance(tokenizer);
        return create_token(TOKEN_REDIR_IN, "<");
    }
    return NULL;
}

t_token *tokenizer_get_next_token(t_tokenizer *tokenizer)
{
    if (!tokenizer)
        return NULL;
    ft_skip_space(tokenizer);
    if (tokenizer->current == '>')
    {
        if (lexer_peek(tokenizer) == '>' )
        {
            lexer_advance(tokenizer);
            lexer_advance(tokenizer);
            return create_token(TOKEN_REDIR_APPEND, ">>");
        }
        lexer_advance(tokenizer);
        return create_token(TOKEN_REDIR_OUT, ">" );
    }
    t_token *token = tokenizer_get_next_token_2(tokenizer);
    if (token)
        return token;
    char *word = extract_complex_word(tokenizer);
    if (word && strlen(word) > 0)
        return create_token(TOKEN_WORD, word);
    lexer_advance(tokenizer);
    return create_token(TOKEN_ERROR, NULL);
}



t_token *token_input_2(t_tokenizer *tokenizer, t_token *tokens, t_token *current)
{
    t_token *new_token;

    while (1)
    {
        new_token = tokenizer_get_next_token(tokenizer);
        if (!new_token)
            break;
        if (new_token->type == TOKEN_EOF)
        {
            gc_free(new_token);
            break;
        }
        if (!tokens)
        {
            tokens = new_token;
            current = tokens;
        }
        else
        {
            current->next = new_token;
            current = new_token;
        }
    }
    tokenizer_free(tokenizer);
    return tokens;
}

t_token *tokenize_input(char *input)
{
    t_tokenizer *tokenizer;
    t_token *tokens;
    t_token *current;

    tokenizer = NULL;
    tokens = NULL;
    current = NULL;

    tokenizer = tokenizer_init(input);
    if (!tokenizer)
        return NULL;

    tokens = token_input_2(tokenizer, tokens, current);
    return tokens;
}