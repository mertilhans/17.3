/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:22 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/19 12:22:39 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	handle_heredoc_quotes(char c, t_expander_state *state,
		t_quote_data *data)
{
	data->result = push_char_res(data->result, c, &data->res_len,
			&data->res_cap);
	if (*(state->quote_char) == 0)
		*(state->quote_char) = c;
	else if (*(state->quote_char) == c)
		*(state->quote_char) = 0;
	(*(state->i))++;
}

static void	handle_heredoc_expansion(char *str, t_expander_state *state,
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

static void	process_heredoc_char(char *str, t_expander_state *state,
		t_quote_data *data)
{
	char	current_char;

	current_char = str[*(state->i)];
	if (current_char == '\'' || current_char == '\"')
		handle_heredoc_quotes(current_char, state, data);
	else if (current_char == '$')
		handle_heredoc_expansion(str, state, data);
	else
	{
		data->result = push_char_res(data->result, current_char, &data->res_len,
				&data->res_cap);
		(*(state->i))++;
	}
}

void	init_expander_state(t_expander_state *state, int *i_ptr,
		char *q_ptr, t_env *env_list)
{
	*i_ptr = 0;
	*q_ptr = 0;
	state->i = i_ptr;
	state->quote_char = q_ptr;
	state->env_list = env_list;
}

char	*expand_heredoc_line(char *str, t_env *env_list)
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
		process_heredoc_char(str, &state, data);
	result = data->result;
	gc_free(data);
	return (result);
}
