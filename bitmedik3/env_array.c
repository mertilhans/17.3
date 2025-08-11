/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/05 17:11:06 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>
#include <string.h>

static int	count_env_entries(t_env *env_list)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env_list;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
	return (count);
}

static char	*create_env_string(t_env *current)
{
	int		len;
	char	*env_str;

	len = strlen(current->key) + strlen(current->value) + 2;
	env_str = gb_malloc(len);
	if (!env_str)
		return (NULL);
	strcpy(env_str, current->key);
	strcat(env_str, "=");
	strcat(env_str, current->value);
	return (env_str);
}

static void	free_env_array_on_error(char **env_array, int i)
{
	int	j;

	j = 0;
	while (j < i)
	{
		env_gc_free(env_array[j]);
		j++;
	}
	env_gc_free(env_array);
}

static char	**fill_env_array(t_env *env_list, int count)
{
	char	**env_array;
	t_env	*current;
	int		i;

	env_array = gb_malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	i = 0;
	current = env_list;
	while (current && i < count)
	{
		if (current->value)
		{
			env_array[i] = create_env_string(current);
			if (!env_array[i])
			{
				free_env_array_on_error(env_array, i);
				return (NULL);
			}
			i++;
		}
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

char	**env_list_to_array(t_env *env_list)
{
	int	count;

	count = count_env_entries(env_list);
	return (fill_env_array(env_list, count));
}
