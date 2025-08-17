/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:29 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:48:30 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	env_bubble_sort(char **env_array, int count)
{
	int		i;
	int		j;
	char	*temp;
	int		swapped;

	if (!env_array || count <= 1)
		return ;
	for (i = 0; i < count - 1; i++)
	{
		swapped = 0;
		for (j = 0; j < count - i - 1; j++)
		{
			if (ft_strcmp(env_array[j], env_array[j + 1]) > 0)
			{
				temp = env_array[j];
				env_array[j] = env_array[j + 1];
				env_array[j + 1] = temp;
				swapped = 1;
			}
		}
		if (swapped == 0)
			break ;
	}
}

char	**create_sorted_env_array(t_env *env_list)
{
	int		count;
	t_env	*current;
	char	**env_array;
	int		i;
	int		len;

	count = 0;
	current = env_list;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
	if (count == 0)
		return (NULL);
	env_array = env_gb_malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	current = env_list;
	i = 0;
	while (current && i < count)
	{
		if (current->value)
		{
			len = ft_strlen(current->key) + ft_strlen(current->value) + 2;
			env_array[i] = env_gb_malloc(len);
			if (!env_array[i])
				return (NULL);
			ft_strcpy(env_array[i], current->key);
			ft_strcat(env_array[i], "=");
			ft_strcat(env_array[i], current->value);
			i++;
		}
		current = current->next;
	}
	env_array[i] = NULL;
	env_bubble_sort(env_array, count);
	return (env_array);
}

void	builtin_env(t_env *env_list)
{
	char **sorted_env;
	int i;

	sorted_env = create_sorted_env_array(env_list);
	i = 0;
	if (!sorted_env)
	{
		set_last_exit_status(0);
		return ;
	}
	while (sorted_env[i])
	{
		printf("%s\n", sorted_env[i]);
		i++;
	}
	i = 0;
	while (sorted_env[i])
	{
		env_gc_free(sorted_env[i]);
		i++;
	}
	env_gc_free(sorted_env);
	set_last_exit_status(0);
}