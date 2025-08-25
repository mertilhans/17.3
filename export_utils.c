/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:16 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/20 18:13:58 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

void	export_print_all(t_export **export_list)
{
	t_export	*current;

	current = *export_list;
	while (current)
	{
		printf("declare -x %s", current->key);
		if (current->value != NULL)
			printf("=\"%s\"", current->value);
		printf("\n");
		current = current->next;
	}
}

char	*export_build_value(t_parser *cmd, int *i, char *value)
{
	char	*full_value;
	int		j;
	int		total_len;

	total_len = ft_strlen(value);
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		total_len += ft_strlen(cmd->argv[j]) + 1;
		j++;
	}
	full_value = gb_malloc(total_len + 1);
	ft_strcpy(full_value, value);
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		ft_strcat(full_value, " ");
		ft_strcat(full_value, cmd->argv[j]);
		j++;
	}
	*i = j - 1;
	return (full_value);
}

int	is_valid_identifier(const char *identifier)
{
	int	i;

	i = 0;
	if (!identifier || identifier[0] == '\0')
		return (0);
	if (!((identifier[0] >= 'a' && identifier[0] <= 'z')
			|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
			|| identifier[0] == '_'))
		return (0);
	i = 1;
	while (identifier[i] && identifier[i] != '=')
	{
		if (!((identifier[i] >= 'a' && identifier[i] <= 'z')
				|| (identifier[i] >= 'A' && identifier[i] <= 'Z')
				|| (identifier[i] >= '0' && identifier[i] <= '9')
				|| identifier[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}
