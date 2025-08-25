/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 16:47:54 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/21 18:22:40 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

static void	handle_quote(t_tokenizer *tokenizer, t_token_data *data,
		char quote_char, char other_quote)
{
	if (tokenizer->current == quote_char && data->in_quote != other_quote)
	{
		if (data->in_quote == quote_char)
			data->in_quote = '\0';
		else
			data->in_quote = quote_char;
	}
}

static char	*process_character(t_tokenizer *tokenizer, t_token_data *data)
{
	if (tokenizer->current == '\'')
	{
		handle_quote(tokenizer, data, '\'', '"');
		data->word = lexer_append_char(tokenizer->current, data);
		if (!data->word)
			return (NULL);
		lexer_advance(tokenizer);
		return (data->word);
	}
	if (tokenizer->current == '"')
	{
		handle_quote(tokenizer, data, '"', '\'');
		data->word = lexer_append_char(tokenizer->current, data);
		if (!data->word)
			return (NULL);
		lexer_advance(tokenizer);
		return (data->word);
	}
	data->word = lexer_append_char(tokenizer->current, data);
	if (!data->word)
		return (NULL);
	lexer_advance(tokenizer);
	return (data->word);
}

static char	*process_word_loop(t_tokenizer *tokenizer, t_token_data *data)
{
	while (tokenizer->current && (data->in_quote
			|| (!ft_ispace(tokenizer->current)
				&& !is_special_char(tokenizer->current))))
	{
		data->word = process_character(tokenizer, data);
		if (!data->word)
			return (NULL);
	}
	return (data->word);
}

char	*extract_complex_word(t_tokenizer *tokenizer)
{
	t_token_data	data;

	ft_token_data_init(&data);
	data.word = gb_malloc(data.capacity);
	if (!data.word)
		return (NULL);
	data.word[0] = '\0';
	if (ft_ispace(tokenizer->current))
		return (data.word);
	data.word = process_word_loop(tokenizer, &data);
	if (!data.word)
		return (NULL);
	if (data.in_quote != '\0')
	{
		printf("quote error\n");
		set_last_exit_status(2);
		return (NULL);
	}
	return (data.word);
}
