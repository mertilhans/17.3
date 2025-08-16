/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extension.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:06 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/16 05:21:57 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

char	*lexer_append_char(char c, t_token_data *data)
{
	char	*new_str;

	if (data->len >= data->capacity - 1)
	{
		data->capacity *= 2;
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

void	ft_token_data_init(t_token_data *data)
{
	data->word = NULL;
	data->capacity = 64;
	data->len = 0;
	data->in_quote = '\0';
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
	while (tokenizer->current &&
			(data.in_quote || (!ft_ispace(tokenizer->current)
						&& !is_special_char(tokenizer->current))))
	{
		data.word = process_character(tokenizer, &data);
		if (!data.word)
			return (NULL);
	}
	if (data.in_quote != '\0')
	{
		printf("quote error\n");
		set_last_exit_status(2); 
		return (NULL);
	}
	return (data.word);
}
