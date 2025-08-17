/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/16 06:03:31 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"


static char	*extract_env_key(char *env_str)
{
	char	*eq_pos;

	eq_pos = ft_strchr(env_str, '=');
	if (!eq_pos)
		return (NULL);
	return (ft_strndup(env_str, eq_pos - env_str));
}

static char	*extract_env_value(char *env_str)
{
	char	*eq_pos;

	eq_pos = ft_strchr(env_str, '=');
	if (!eq_pos)
		return (NULL);
	return (ft_strdup(eq_pos + 1));
}

static t_env	*create_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = env_gb_malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

static void	add_env_to_list(t_env **env_list, t_env *new_node)
{
	new_node->next = *env_list;
	*env_list = new_node;
}

t_env	*init_env(char **env)
{
	t_env	*env_list;
	t_env	*new_node;
	char	*key;
	char	*value;
	int		i;

	env_list = NULL;
	i = 0;
	while (env[i])
	{
		if (ft_strchr(env[i], '='))
		{
			key = extract_env_key(env[i]);
			value = extract_env_value(env[i]);
			new_node = create_env_node(key, value);
			if (new_node)
				add_env_to_list(&env_list, new_node);
		}
		i++;
	}
	return (env_list);
}
