/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_process.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:25 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:50:26 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	process_heredocs(t_parser *cmd, t_env *env_list)
{
	t_heredoc_data	h_data;

	h_data.heredoc_content = NULL;
	h_data.heredoc_len = 0;
	h_data.last_heredoc_fd = -2;
	if (h_loop(cmd, &h_data, env_list) == -1)
		return -1;
	return (h_data.last_heredoc_fd);
}

int	h_loop(t_parser *cmd, t_heredoc_data *data, t_env *env_list)
{
	t_redirection	*current_redir;

	current_redir = cmd->redirs;
	while (current_redir)
	{
		if (current_redir->type == REDIR_HEREDOC)
		{
			if (data->last_heredoc_fd != -2 && data->last_heredoc_fd != -1)
			{
				close(data->last_heredoc_fd);
				data->last_heredoc_fd = -1;
			}
			if (ft_h_built_expand(current_redir, data, env_list) == -1)
				return -1;
		}
		current_redir = current_redir->next;
	}
	return 0;
}

int	ft_h_built_expand(t_redirection *current_redir, t_heredoc_data *data,
		t_env *env_list)
{
	int	pipefd[2];

	if (current_redir->no_expand)
		data->heredoc_content = read_single_heredoc_block(current_redir->filename);
	else
		data->heredoc_content = read_heredoc_with_expand(current_redir->filename,
				env_list);
	if (!data->heredoc_content && get_last_exit_status() == 130)
		return -1;
	if (!data->heredoc_content)
		data->heredoc_content = ft_strdup("");
	data->heredoc_len = ft_strlen(data->heredoc_content);
	if (pipe(pipefd) == -1)
	{
		perror("heredoc pipe");
		gc_free(data->heredoc_content);
		set_last_exit_status(1);
		return -1;
	}
	if (data->heredoc_len > 0)
		write(pipefd[1], data->heredoc_content, data->heredoc_len);
	close(pipefd[1]);
	gc_free(data->heredoc_content);
	data->last_heredoc_fd = pipefd[0];
	return (pipefd[0]);
}

char	*read_single_heredoc_block(char *delimiter)
{
	t_heredoc_buffer	buf;
	char				*result;

	if (!delimiter || *delimiter == '\0')
	{
		printf("Heredoc delimiter error\n");
		return NULL;
	}
	ft_h_buffer_init(&buf, delimiter);
	if (!buf.content)
	{
		perror("Heredoc malloc error");
		return NULL;
	}
	result = readline_loop(&buf, delimiter);
	if (!result && get_last_exit_status() == 130)
	{
		gc_free(buf.content);
		return NULL;
	}
	if (result)
		result = buf.content;
	return (result);
}

char	*read_heredoc_with_expand(char *delimiter, t_env *env_list)
{
	t_heredoc_buffer	buf;
	char				*result;

	if (!delimiter || *delimiter == '\0')
	{
		printf("Heredoc delimiter error\n");
		set_last_exit_status(1);
		return NULL;
	}
	ft_h_buffer_init(&buf, delimiter);
	if (!buf.content)
	{
		perror("Heredoc malloc error");
		set_last_exit_status(1);
		return NULL;
	}
	result = readline_loop_expand(&buf, delimiter, env_list);
	if (!result && get_last_exit_status() == 130)
	{
		gc_free(buf.content);
		return NULL;
	}
	if (result)
		result = buf.content;
	return (result);
}