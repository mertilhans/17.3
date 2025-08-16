/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:35:51 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/16 03:38:32 by merilhan         ###   ########.fr       */
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
	if (word && strlen(word) > 0)
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
	tokenizer_free(tokenizer);
	return (tokens);
}

int	only_spaces_before(char *start, char *pos)
{
	while (start < pos)
	{
		if (!ft_ispace(*start))
			return (0);
		start++;
	}
	return (1);
}

int	ft_tokenize_control(char *input)
{
	char	*current;
	char	*temp;

	current = input;
	while (*current)
	{
		if (*current == '|')
		{
			if (only_spaces_before(input, current))
			{
				printf("bash: syntax error near unexpected token `|'\n");
				set_last_exit_status(2);
				return (1);
			}
			temp = current + 1;
			while (*temp && ft_ispace(*temp))
				temp++;
			if (*temp == '\0')
			{
				printf("bash: syntax error near unexpected token `|'\n");
				set_last_exit_status(2);
				return (1);
			}
		}
		current++;
	}
	return (0);
}

t_token	*tokenize_input(char *input)
{
	if (ft_tokenize_control(input))
		return (NULL);

	t_tokenizer *tokenizer = tokenizer_init(input);
	if (!tokenizer)
		return (NULL);

	t_token *tokens = token_input_2(tokenizer, NULL, NULL);
	return (tokens);
}