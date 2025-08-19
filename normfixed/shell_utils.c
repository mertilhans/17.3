/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 04:17:03 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 18:35:18 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_tokenizer	*tokenizer_init(char *input)
{
	t_tokenizer	*tokenizer;

	if (!input)
		return (NULL);
	tokenizer = gb_malloc(sizeof(t_tokenizer));
	if (!tokenizer)
		return (NULL);
	tokenizer->input = ft_strdup(input);
	if (!tokenizer->input)
	{
		gc_free(tokenizer);
		return (NULL);
	}
	tokenizer->pos = 0;
	tokenizer->len = ft_strlen(input);
	if (tokenizer->len > 0)
		tokenizer->current = input[0];
	else
		tokenizer->current = '\0';
	return (tokenizer);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = gb_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = ft_strdup(value);
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

void	cleanup_env_array(char **env_array)
{
	int	i;

	if (!env_array)
		return ;
	i = 0;
	while (env_array[i])
	{
		env_gc_free(env_array[i]);
		i++;
	}
	env_gc_free(env_array);
}

t_parser	*parse_line(char *line, t_env *env_list)
{
	t_token		*tokens;
	t_parser	*cmd_list;

	tokens = tokenize_input(line);
	if (!tokens)
	{
		setup_interactive_signals();
		return (NULL);
	}
	cmd_list = parse_tokens(tokens, env_list);
	if (!cmd_list)
	{
		setup_interactive_signals();
		return (NULL);
	}
	return (cmd_list);
}
