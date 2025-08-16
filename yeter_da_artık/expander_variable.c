/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_variable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 02:47:36 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 02:47:49 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char	*extract_var_name(char *str, int *i)
{
	char	*var_name;
	int		start_i;
	int		len;

	start_i = *i;
	while (str[*i] && (isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	len = *i - start_i;
	if (len <= 0)
		return (ft_strdup(""));
	var_name = gb_malloc(len + 1);
	if (!var_name)
		return (ft_strdup(""));
	ft_memcpy(var_name, str + start_i, len);
	var_name[len] = '\0';
	return (var_name);
}

static char	*search_env_variable(char *var_name, t_env *env_list)
{
	t_env	*env_node;
	char	*expanded;

	env_node = find_env(env_list, var_name);
	if (env_node && env_node->value)
	{
		expanded = ft_strdup(env_node->value);
		return (expanded ? expanded : ft_strdup(""));
	}
	return (NULL);
}

static char	*search_export_variable(char *var_name)
{
	char	*export_value;
	char	*expanded;

	export_value = find_export_value(var_name);
	if (export_value)
	{
		expanded = ft_strdup(export_value);
		return (expanded ? expanded : ft_strdup(""));
	}
	return (ft_strdup(""));
}

static char	*search_variable(char *var_name, t_env *env_list)
{
	char	*result;

	result = search_env_variable(var_name, env_list);
	if (result)
		return (result);
	return (search_export_variable(var_name));
}

char	*handle_dollar(char *str, int *i, t_env *env_list)
{
	char *var_name;
	char *result;

	(*i)++;
	if (!str[*i])
		return (ft_strdup("$"));
	if (str[*i] == '?' || str[*i] == '$')
	{
		result = handle_special_vars(str[*i]);
		if (result)
		{
			(*i)++;
			return (result);
		}
	}
	if (isalpha(str[*i]) || str[*i] == '_')
	{
		var_name = extract_var_name(str, i);
		result = search_variable(var_name, env_list);
		gc_free(var_name);
		return (result);
	}
	return (ft_strdup("$"));
}