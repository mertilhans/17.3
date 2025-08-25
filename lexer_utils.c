/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 16:46:45 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/21 18:21:19 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_token	*tokenizer_get_next_token_2(t_tokenizer *tokenizer)
{
	if (tokenizer->current == '\0')
		return (create_token(TOKEN_EOF, NULL));
	if (tokenizer->current == '|')
	{
		lexer_advance(tokenizer);
		return (create_token(TOKEN_PIPE, "|"));
	}
	if (tokenizer->current == '<')
	{
		if (lexer_peek(tokenizer) == '<')
		{
			lexer_advance(tokenizer);
			lexer_advance(tokenizer);
			return (create_token(TOKEN_HEREDOC, "<<"));
		}
		lexer_advance(tokenizer);
		return (create_token(TOKEN_REDIR_IN, "<"));
	}
	return (NULL);
}

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

char	*lexer_append_char(char c, t_token_data *data)
{
	char	*new_str;

	if (data->len >= data->capacity - 1)
	{
		data->capacity += 1;
		new_str = gb_malloc(data->capacity);
		if (!new_str)
			return (NULL);
		if (data->word)
			ft_memcpy(new_str, data->word, data->len);
		data->word = new_str;
	}
	data->word[(data->len)++] = c;
	data->word[data->len] = '\0';
	return (data->word);
}

void	ft_token_data_init(t_token_data *data)
{
	data->word = NULL;
	data->capacity = 1;
	data->len = 0;
	data->in_quote = '\0';
}
