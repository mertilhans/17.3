#include "shell.h"

static int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

static char	*append_char(char c, t_token_data *data)
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
		data->word = append_char(tokenizer->current, data);
		if (!data->word)
			return (NULL);
		lexer_advance(tokenizer);
		return (data->word);
	}
	if (tokenizer->current == '"')
	{
		handle_quote(tokenizer, data, '"', '\'');
		data->word = append_char(tokenizer->current, data);
		if (!data->word)
			return (NULL);
		lexer_advance(tokenizer);
		return (data->word);
	}
	data->word = append_char(tokenizer->current, data);
	if (!data->word)
		return (NULL);
	lexer_advance(tokenizer);
	return (data->word);
}

char	*extract_complex_word(t_tokenizer *tokenizer)
{
	t_token_data	data;

	data.word = NULL;
	data.capacity = 64;
	data.len = 0;
	data.in_quote = '\0';
	data.word = gb_malloc(data.capacity);
	if (!data.word)
		return (NULL);
	data.word[0] = '\0';
	if (ft_ispace(tokenizer->current))
		return (data.word);
	while (tokenizer->current && (data.in_quote
			|| (!ft_ispace(tokenizer->current)
				&& !is_special_char(tokenizer->current))))
	{
		data.word = process_character(tokenizer, &data);
		if (!data.word)
			return (NULL);
	}
	if (data.in_quote != '\0')
	{
		printf("bash: syntax error: unterminated quoted string\n");
		set_last_exit_status(2);
		gc_free(data.word);
		return (NULL);
	}
	return (data.word);
}