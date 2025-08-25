/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_quotes2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:26 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/19 12:22:45 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	handle_quotes(char *str, t_expander_state *state,
		t_quote_data *data)
{
	if (str[*(state->i)] == '\'' || str[*(state->i)] == '"')
	{
		if (*(state->quote_char) == 0
			|| *(state->quote_char) == str[*(state->i)])
			process_quote_char(str, state->i, state->quote_char);
		else
		{
			data->result = push_char_res(data->result, str[*(state->i)],
					&data->res_len, &data->res_cap);
			(*(state->i))++;
		}
	}
}

static void	handle_expansion(char *str, t_expander_state *state,
		t_quote_data *data)
{
	char	*expanded;

	expanded = handle_dollar(str, state->i, state->env_list);
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

static void	process_quote_loop(char *str, t_expander_state *state,
		t_quote_data *data)
{
	if (str[*(state->i)] == '\'' || str[*(state->i)] == '"')
		handle_quotes(str, state, data);
	else if (str[*(state->i)] == '$' && *(state->quote_char) != '\'')
		handle_expansion(str, state, data);
	else
	{
		data->result = push_char_res(data->result, str[*(state->i)],
				&data->res_len, &data->res_cap);
		(*(state->i))++;
	}
}

char	*expand_with_quotes(char *str, t_env *env_list)
{
	t_quote_data		*data;
	char				*result;
	t_expander_state	state;
	int					i;
	char				quote_char;

	if (!str)
		return (NULL);
	data = init_quote_data(str);
	if (!data)
		return (NULL);
	init_expander_state(&state, &i, &quote_char, env_list);
	while (str[i])
		process_quote_loop(str, &state, data);
	result = data->result;
	gc_free(data);
	return (result);
}
