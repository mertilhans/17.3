/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:29 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:50:30 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	heredoc_append_line(t_heredoc_buffer *buf)
{
	size_t	line_len;

	line_len = ft_strlen(buf->line);
	buf->line_with_nl = gb_malloc(line_len + 2);
	if (!buf->line_with_nl)
		return (0);
	ft_strcpy(buf->line_with_nl, buf->line);
	buf->line_with_nl[line_len] = '\n';
	buf->line_with_nl[line_len + 1] = '\0';
	buf->new_content = gb_malloc(ft_strlen(buf->content)
			+ ft_strlen(buf->line_with_nl) + 1);
	if (!buf->new_content)
	{
		gc_free(buf->line_with_nl);
		return (0);
	}
	ft_strcpy(buf->new_content, buf->content);
	ft_strcat(buf->new_content, buf->line_with_nl);
	gc_free(buf->content);
	gc_free(buf->line_with_nl);
	buf->content = buf->new_content;
	return (1);
}

int	heredoc_append_expanded(t_heredoc_buffer *buf, t_env *env_list)
{
	char	*expanded_line;
	char	*old_line;
	int		result;

	old_line = buf->line;
	expanded_line = expand_heredoc_line_impl(buf->line, env_list);
	buf->line = expanded_line;
	result = heredoc_append_line(buf);
	buf->line = old_line;
	if (expanded_line != old_line)
		gc_free(expanded_line);
	return (result);
}

char	*expand_heredoc_line_impl(char *line, t_env *env_list)
{
	char	*expanded;

	if (!line)
		return (NULL);
	expanded = expand_heredoc_line(line, env_list);
	if (expanded)
		return (expanded);
	else
		return (ft_strdup(line));
}

void	ft_h_buffer_init(t_heredoc_buffer *buf, char *delimiter)
{
	buf->content = ft_strdup("");
	buf->line = NULL;
	buf->line_with_nl = NULL;
	buf->new_content = NULL;
	buf->delimiter_len = ft_strlen(delimiter);
}

int	heredoc_handle_init(t_parser *cmd_list)
{
	t_parser	*tmp_cmd;

	tmp_cmd = cmd_list;
	while (tmp_cmd)
	{
		if (tmp_cmd->heredoc_fd != -1 && tmp_cmd->heredoc_fd != -2)
		{
			close(tmp_cmd->heredoc_fd);
			tmp_cmd->heredoc_fd = -1;
		}
		tmp_cmd = tmp_cmd->next;
	}
	return (1);
}