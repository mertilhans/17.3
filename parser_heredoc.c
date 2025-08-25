/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 17:44:38 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/22 23:36:50 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	has_quote_chars(char *delimiter)
{
	int	i;
	int	len;

	i = 0;
	len = ft_strlen(delimiter);
	while (i < len)
	{
		if (delimiter[i] == '"' || delimiter[i] == '\'')
			return (1);
		i++;
	}
	return (0);
}

char	*remove_quotes(char *d)
{
	char	*clean_delimiter;
	int		i;
	int		j;
	int		len;
	int		a;

	len = ft_strlen(d);
	clean_delimiter = gb_malloc(len + 1);
	if (!clean_delimiter)
		return (NULL);
	i = 0;
	j = 0;
	a = 0;
	while (i < len)
	{
		if (d[a] == '$' && (d[a + 1] == '\'' || d[a + 1] == '"'))
			i++;
		if (d[a] == '$' && (d[a + 1] == '\'' || d[a + 1] == '"'))
			a++;
		if (d[i] != '"' && d[i] != '\'')
			clean_delimiter[j++] = d[i];
		i++;
	}
	clean_delimiter[j] = '\0';
	return (clean_delimiter);
}
