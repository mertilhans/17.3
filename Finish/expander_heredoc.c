/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:22 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:23 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	handle_heredoc_quotes(char *str, int *i, char *quote_char,
		t_quote_data *data)
{
	data->result = push_char_res(data->result, str[*i], &data->res_len,
			&data->res_cap);
	if (*quote_char == 0)
		*quote_char = str[*i];
	else if (*quote_char == str[*i])
		*quote_char = 0;
	(*i)++;
}

static void	handle_heredoc_expansion(char *str, int *i, t_env *env_list,
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

static void	process_heredoc_char(char *str, int *i, char *quote_char,
		t_env *env_list, t_quote_data *data)
{
	if (str[*i] == '\'' || str[*i] == '\"')
		handle_heredoc_quotes(str, i, quote_char, data);
	else if (str[*i] == '$')
		handle_heredoc_expansion(str, i, env_list, data);
	else
	{
		data->result = push_char_res(data->result, str[*i], &data->res_len,
				&data->res_cap);
		(*i)++;
	}
}

char	*expand_heredoc_line(char *str, t_env *env_list)
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
		process_heredoc_char(str, &i, &quote_char, env_list, data);
	result = data->result;
	gc_free(data);
	return (result);
}