/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 16:47:32 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:28:05 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// --- MISSING LEXER UTILS IMPLEMENTATIONS ---
#include <stdlib.h>
#include "shell.h"

void ft_skip_space(t_tokenizer *tokenizer)
{
	while (tokenizer->current && ft_ispace(tokenizer->current))
		lexer_advance(tokenizer);
}

char lexer_peek(t_tokenizer *tokenizer)
{
	if (tokenizer->pos + 1 < tokenizer->len)
		return tokenizer->input[tokenizer->pos + 1];
	return '\0';
}

void lexer_advance(t_tokenizer *tokenizer)
{
	if (tokenizer->pos < tokenizer->len)
	{
		tokenizer->pos++;
		tokenizer->current = tokenizer->input[tokenizer->pos];
	}
}

void tokenizer_free(t_tokenizer *tokenizer)
{
	if (tokenizer)
		free(tokenizer);
}

int ft_ispace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

