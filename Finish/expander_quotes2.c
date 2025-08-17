/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_quotes2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:26 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:27 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	handle_quotes(char *str, int *i, char *quote_char,
		t_quote_data *data)
{
	if (str[*i] == '\'' || str[*i] == '"')
	{
		if (*quote_char == 0 || *quote_char == str[*i])
			process_quote_char(str, i, quote_char);
		else
		{
			data->result = push_char_res(data->result, str[*i], &data->res_len,
					&data->res_cap);
			(*i)++;
		}
	}
}

static void	handle_expansion(char *str, int *i, t_env *env_list,
		t_quote_data *data)
{
	char	*expanded;

	expanded = handle_dollar(str, i, env_list);
	if (expanded)
	{
		data->result = append_string_res(data->result, expanded, &data->res_len,
				&data->res_cap);
		gc_free(expanded);
	}
}

t_quote_data	*init_quote_data(char *str)
{
	t_quote_data	*data;

	data = gb_malloc(sizeof(t_quote_data));
	if (!data)
		return (NULL);
	data->result = gb_malloc(ft_strlen(str) + 1);
	if (!data->result)
		return (NULL);
	data->result[0] = '\0';
	data->res_len = 0;
	data->res_cap = ft_strlen(str) + 1;
	return (data);
}

static void	process_quote_loop(char *str, int *i, char *quote_char,
		t_env *env_list, t_quote_data *data)
{
	if (str[*i] == '\'' || str[*i] == '"')
		handle_quotes(str, i, quote_char, data);
	else if (str[*i] == '$' && *quote_char != '\'')
		handle_expansion(str, i, env_list, data);
	else
	{
		data->result = push_char_res(data->result, str[*i], &data->res_len,
				&data->res_cap);
		(*i)++;
	}
}
char	*expand_with_quotes(char *str, t_env *env_list)
{
	int i;
	char quote_char;
	t_quote_data *data;
	char *result;

	if (!str)
		return (NULL);
	data = init_quote_data(str);
	if (!data)
		return (NULL);
	i = 0;
	quote_char = 0;
	while (str[i])
		process_quote_loop(str, &i, &quote_char, env_list, data);
	result = data->result;
	gc_free(data);
	return (result);
}