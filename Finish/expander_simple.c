/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_simple.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:29 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:30 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char	*create_var_name(const char *str, int *i, int len)
{
	char	*var_name;

	var_name = gb_malloc(len + 1);
	if (!var_name)
		return (NULL);
	ft_memcpy(var_name, str + *i, len);
	var_name[len] = '\0';
	*i += len;
	return (var_name);
}

static char	*process_simple_var(const char *str, int *i, int len,
		t_env *env_list)
{
	char	*var_name;
	t_env	*env_var;
	char	*result;

	var_name = create_var_name(str, i, len);
	if (!var_name)
		return (NULL);
	env_var = find_env(env_list, var_name);
	if (env_var == NULL)
	{
		result = ft_strdup("");
		gc_free(var_name);
		return (result);
	}
	gc_free(var_name);
	return (get_env_value(env_var, var_name));
}

char	*handle_simple(const char *str, int *i, t_env *env_list)
{
	int len;

	(*i)++;
	len = 0;
	while (is_valid_char(str[*i + len]))
		len++;
	if (len == 0)
	{
		(*i)++;
		return (ft_strdup(""));
	}
	return (process_simple_var(str, i, len, env_list));
}