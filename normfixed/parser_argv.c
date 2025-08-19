/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_argv.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 17:31:56 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 17:43:59 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	expand_argv_capacity(t_parser *cmd, int *argc)
{
	char	**new_argv;
	int		i;

	cmd->argv_cap *= 2;
	new_argv = gb_malloc(sizeof(char *) * cmd->argv_cap);
	if (!new_argv)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < *argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	gc_free(cmd->argv);
	cmd->argv = new_argv;
}

int	is_empty_expansion(char *original_token, char *expanded)
{
	if (!expanded || expanded[0] != '\0')
		return (0);
	if (ft_strchr(original_token, '"') || ft_strchr(original_token, '\''))
		return (0);
	if (ft_strlen(original_token) <= 1 || original_token[0] != '$')
		return (0);
	if (!isalnum(original_token[1]) && original_token[1] != '_')
		return (0);
	return (1);
}

int	should_split_expansion(char *original_token, char *expanded)
{
	int	has_dollar;
	int	has_quote;

	if (!expanded)
		return (0);
	has_dollar = (ft_strchr(original_token, '$') != NULL);
	if (!has_dollar)
		return (0);
	has_quote = (ft_strchr(original_token, '"') != NULL
			|| ft_strchr(original_token, '\'') != NULL);
	if (has_quote)
		return (0);
	if (!ft_strchr(expanded, ' ') && !ft_strchr(expanded, '\t'))
		return (0);
	return (1);
}

void	add_split_args(t_parser *cmd, int *argc, char **split_args)
{
	int	j;

	j = 0;
	while (split_args && split_args[j])
	{
		if (*argc + 1 >= cmd->argv_cap)
			expand_argv_capacity(cmd, argc);
		cmd->argv[*argc] = ft_strdup(split_args[j]);
		if (!cmd->argv[*argc])
		{
			perror("strdup failed for split arg");
			exit(EXIT_FAILURE);
		}
		(*argc)++;
		j++;
	}
}

void	cleanup_split_args(char **split_args)
{
	int	j;

	if (!split_args)
		return ;
	j = 0;
	while (split_args[j])
	{
		gc_free(split_args[j]);
		j++;
	}
	gc_free(split_args);
}
