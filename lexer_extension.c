/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extension.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 16:20:05 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/21 18:18:49 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

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

static void	lexer_syntax(void)
{
	printf("bash: syntax error near unexpected token `|'\n");
	set_last_exit_status(2);
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
				lexer_syntax();
				return (1);
			}
			temp = current + 1;
			while (*temp && ft_ispace(*temp))
				temp++;
			if (*temp == '\0')
			{
				lexer_syntax();
				return (1);
			}
		}
		current++;
	}
	return (0);
}
