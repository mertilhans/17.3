/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_loop.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:22 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 23:39:45 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char	*readline_loop(t_heredoc_buffer *buf, const char *delimiter)
{
	char	*result;

	heredoc_signals();
	while (1)
	{
		buf->line = heredoc_readline("> ");
		if (!buf->line)
		{
			if (process_null_line(buf, delimiter, &result))
				return (result);
		}
		if (check_delimiter(buf, delimiter))
		{
			setup_interactive_signals();
			break ;
		}
		if (!process_line(buf, NULL, 0))
		{
			setup_interactive_signals();
			return (NULL);
		}
	}
	setup_interactive_signals();
	return (buf->content);
}

char	*readline_loop_expand(t_heredoc_buffer *buf, const char *delimiter,
		t_env *env_list)
{
	char	*result;

	heredoc_signals();
	while (1)
	{
		buf->line = heredoc_readline("> ");
		if (!buf->line)
		{
			if (process_null_line(buf, delimiter, &result))
				return (result);
		}
		if (check_delimiter(buf, delimiter))
		{
			setup_interactive_signals();
			break ;
		}
		if (!process_line(buf, env_list, 1))
		{
			setup_interactive_signals();
			return (NULL);
		}
	}
	setup_interactive_signals();
	return (buf->content);
}

int	process_null_line(t_heredoc_buffer *buf, const char *delimiter,
		char **result)
{
	if (get_last_exit_status() == 130)
	{
		setup_interactive_signals();
		*result = NULL;
		return (1);
	}
	printf("bash: warning: here-document at line  delimited by end-of-file"
		" (wanted `%s')\n", delimiter);
	set_last_exit_status(0);
	setup_interactive_signals();
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
