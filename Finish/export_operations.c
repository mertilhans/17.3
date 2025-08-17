/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_operations.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/16 06:05:39 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>


void	unset_export_value(t_export **export_list, const char *key)
{
	t_export	*current;
	t_export	*prev;

	current = *export_list;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*export_list = current->next;
			if (current->key)
				free(current->key);
			if (current->value)
				free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

void	free_export_list(t_export *export_list)
{
	t_export	*current;
	t_export	*next;

	current = export_list;
	while (current)
	{
		next = current->next;
		if (current->key)
			free(current->key);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}

t_export	*init_export_from_env(t_env *env_list)
{
	t_export	*export_list;
	t_env		*current;

	export_list = NULL;
	current = env_list;
	while (current)
	{
		set_export_value(&export_list, current->key, current->value);
		current = current->next;
	}
	return (export_list);
}
