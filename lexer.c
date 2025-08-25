/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:35:51 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/22 19:14:57 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_token	*tokenizer_get_next_token(t_tokenizer *tokenizer)
{
	char	*word;
	t_token	*token;

	if (!tokenizer)
		return (NULL);
	ft_skip_space(tokenizer);
	if (tokenizer->current == '>')
	{
		if (lexer_peek(tokenizer) == '>')
		{
			lexer_advance(tokenizer);
			lexer_advance(tokenizer);
			return (create_token(TOKEN_REDIR_APPEND, ">>"));
		}
		lexer_advance(tokenizer);
		return (create_token(TOKEN_REDIR_OUT, ">"));
	}
	token = tokenizer_get_next_token_2(tokenizer);
	if (token)
		return (token);
	word = extract_complex_word(tokenizer);
	if (word && ft_strlen(word) > 0)
		return (create_token(TOKEN_WORD, word));
	return (create_token(TOKEN_ERROR, NULL));
}

t_token	*token_input_2(t_tokenizer *tokenizer, t_token *tokens,
		t_token *current)
{
	t_token	*new_token;

	while (1)
	{
		new_token = tokenizer_get_next_token(tokenizer);
		if (!new_token)
			break ;
		if (new_token->type == TOKEN_EOF)
		{
			gc_free(new_token);
			break ;
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
	return (tokens);
}

t_token	*tokenize_input(char *input)
{
	t_tokenizer	*tokenizer;
	t_token		*tokens;
	t_token		*tmp;

	if (ft_tokenize_control(input))
		return (NULL);
	tokenizer = tokenizer_init(input);
	if (!tokenizer)
		return (NULL);
	tokens = token_input_2(tokenizer, NULL, NULL);
	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == TOKEN_ERROR)
			return (NULL);
		tmp = tmp->next;
	}
	return (tokens);
}
