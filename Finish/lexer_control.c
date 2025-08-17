/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_control.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 04:41:43 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:28:03 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

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

static int	handle_pipe_syntax_error(char *input, char *current)
{
	char	*temp;

	if (only_spaces_before(input, current))
	{
		printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	temp = current + 1;
	while (*temp && ft_ispace(*temp))
		temp++;
	if (*temp == '\0' || *temp == '|')
	{
		printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}

int	ft_tokenize_control(char *input)
{
	char *current = input;

	while (*current)
	{
		if (*current == '|')
		{
			if (handle_pipe_syntax_error(input, current))
				return (1);
			// Basit bir örnek: başta veya sonda pipe varsa hata döndür
			if (current == input || *(current + 1) == '\0')
				return (1);
		}
		current++;
	}
	return (0);
}