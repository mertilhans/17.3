/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_loop.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:22 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/25 19:34:22 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int	process_null_line(t_heredoc_buffer *buf, const char *delimiter,
		char **result)
{
	if (g_signal_status == SIGINT)
	{
		ft_init_signals(PROMPT);
		*result = NULL;
		return (1);
	}
	printf("bash: warning: here-document at line  delimited by end-of-file"
		" (wanted `%s')\n", delimiter);
	set_last_exit_status(0);
	ft_init_signals(PROMPT);
	*result = buf->content;
	return (1);
}

int	process_line(t_heredoc_buffer *buf, t_env *env_list, int expand_mode)
{
	int	result;

	if (expand_mode)
		result = heredoc_append_expanded(buf, env_list);
	else
		result = heredoc_append_line(buf);
	if (!result)
	{
		not_her_app_exp(buf);
		return (0);
	}
	gc_free(buf->line);
	buf->line = NULL;
	return (1);
}

int	check_delimiter(t_heredoc_buffer *buf, const char *delimiter)
{
	if ((size_t)ft_strlen(buf->line) == buf->delimiter_len
		&& ft_strcmp(buf->line, delimiter) == 0)
	{
		gc_free(buf->line);
		return (1);
	}
	return (0);
}

char	*readline_loop(t_heredoc_buffer *buf, const char *delimiter)
{
	char	*result;
	char	*res;

	ft_init_signals(HEREDOC);
	while (1)
	{
		res = readline("> ");
		if (g_signal_status == SIGINT)
		{
			free(res);
			gc_free(buf->content);
			ft_init_signals(PROMPT);
			g_signal_status = 0;
			return (NULL);
		}
		if (!res)
		{
			if (process_null_line(buf, delimiter, &result))
				return (result);
		}
		buf->line = ft_strdup(res);
		free(res);
		if (check_delimiter(buf, delimiter))
			break ;
		if (!process_line(buf, NULL, 0))
			return (NULL);
	}
	ft_init_signals(PROMPT);
	return (buf->content);
}

char	*readline_loop_expand(t_heredoc_buffer *buf, const char *delimiter,
		t_env *env_list)
{
	char	*result;
	char	*res;

	ft_init_signals(HEREDOC);
	while (1)
	{
		res = readline("> ");
		if (g_signal_status == SIGINT)
		{
			free(res);
			gc_free(buf->content);
			ft_init_signals(PROMPT);
			g_signal_status = 0; 
			return (NULL);
		}
		if (!res)
		{
			if (process_null_line(buf, delimiter, &result))
				return (result);
		}
		buf->line = ft_strdup(res);
		free(res);
		if (check_delimiter(buf, delimiter))
			break ;
		if (!process_line(buf, env_list, 1))
			return (NULL);
	}
	ft_init_signals(PROMPT);
	return (buf->content);
}
