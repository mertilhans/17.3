/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_operations.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/16 02:21:21 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>
#include <string.h>

static void	update_existing_env(t_env *existing, char *value)
{
	env_gc_free(existing->value);
	existing->value = ft_strdup(value);
}

static t_env	*create_new_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = env_gb_malloc(sizeof(t_env));
	if (!new_node)
	{
		perror("malloc failed");
		return (NULL);
	}
	new_node->key = ft_strdup(key);
	new_node->value = ft_strdup(value);
	new_node->next = NULL;
	return (new_node);
}

void	set_env_value(t_env **env_list, char *key, char *value)
{
	t_env	*existing;
	t_env	*new_node;

	if (!env_list)
	{
		return ;
	}
	existing = find_env(*env_list, key);
	if (existing)
	{
		update_existing_env(existing, value);
		return ;
	}
	new_node = create_new_env_node(key, value);
	if (!new_node)
		return ;
	new_node->next = *env_list;
	*env_list = new_node;
}

static void	remove_env_node(t_env **env_list, t_env *prev, t_env *current)
{
	if (prev)
		prev->next = current->next;
	else
		*env_list = current->next;
	env_gc_free(current->key);
	env_gc_free(current->value);
	env_gc_free(current);
}

void	unset_env_value(t_env **env_list, char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!env_list || !*env_list || !key)
		return ;
	current = *env_list;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			remove_env_node(env_list, prev, current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
