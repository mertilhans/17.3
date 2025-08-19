/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extra.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 17:27:54 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 17:27:55 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	ft_tokenize_control(char *input)
{
	char	*current;

	current = input;
	while (*current)
	{
		if (*current == '|')
		{
			if (current == input || *(current + 1) == '\0')
				return (1);
		}
		current++;
	}
	return (0);
}
