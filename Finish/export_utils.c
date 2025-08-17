/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:16 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:50:17 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

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
	if (!identifier || identifier[0] == '\0')
		return (0);
	if (!((identifier[0] >= 'a' && identifier[0] <= 'z')
			|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
			|| identifier[0] == '_'))
		return (0);
	return (1);
}