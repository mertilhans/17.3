/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 16:47:32 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 17:28:09 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>

void	ft_skip_space(t_tokenizer *tokenizer)
{
	while (tokenizer->current && ft_ispace(tokenizer->current))
		lexer_advance(tokenizer);
}

char	lexer_peek(t_tokenizer *tokenizer)
{
	if (tokenizer->pos + 1 < tokenizer->len)
		return (tokenizer->input[tokenizer->pos + 1]);
	return ('\0');
}

void	lexer_advance(t_tokenizer *tokenizer)
{
	if (tokenizer->pos < tokenizer->len)
	{
		tokenizer->pos++;
		tokenizer->current = tokenizer->input[tokenizer->pos];
	}
}

void	tokenizer_free(t_tokenizer *tokenizer)
{
	if (tokenizer)
		free(tokenizer);
}

int	ft_ispace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
		|| c == '\r');
}
