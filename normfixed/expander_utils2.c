/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:33 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 15:42:31 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char	*append_expansion(char *res, char *expanded)
{
	int	res_len;
	int	add_len;

	res_len = ft_strlen(res);
	add_len = ft_strlen(expanded);
	res = realloc_result(res, res_len + add_len);
	if (!res)
		return (NULL);
	ft_memcpy(res + res_len, expanded, add_len);
	res[res_len + add_len] = '\0';
	return (res);
}

static char	*handle_dollar_expansion(char *str, int *i, t_env *env_list,
		char *res)
{
	char	*expanded;

	expanded = handle_dollar(str, i, env_list);
	if (!expanded)
		return (res);
	res = append_expansion(res, expanded);
	gc_free(expanded);
	return (res);
}

static char	*handle_regular_char(char *res, char c)
{
	char	*new_res;
	int		res_len;

	res_len = ft_strlen(res);
	new_res = gb_malloc(res_len + 2);
	if (!new_res)
	{
		gc_free(res);
		return (NULL);
	}
	ft_memcpy(new_res, res, res_len);
	new_res[res_len] = c;
	new_res[res_len + 1] = '\0';
	gc_free(res);
	return (new_res);
}

char	*expand_variables(char *str, t_env *env_list)
{
	char	*res;
	int		i;

	if (!str)
		return (NULL);
	res = gb_malloc(ft_strlen(str) + 1);
	if (!res)
		return (NULL);
	res[0] = '\0';
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
			res = handle_dollar_expansion(str, &i, env_list, res);
		else
		{
			res = handle_regular_char(res, str[i]);
			i++;
		}
		if (!res)
			return (NULL);
	}
	return (res);
}

char	*handle_special_vars(char c)
{
	if (c == '?')
		return (ft_itoa(get_last_exit_status()));
	if (c == '$')
		return (ft_itoa(getpid()));
	return (NULL);
}
