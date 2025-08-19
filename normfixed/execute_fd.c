/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_fd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:59 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 15:27:00 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

void	close_heredoc_fd(t_parser *cmd)
{
	t_parser	*cmd_list;

	cmd_list = cmd;
	while (cmd_list)
	{
		if (cmd_list->heredoc_fd != -2 && cmd_list->heredoc_fd != -1)
		{
			close(cmd_list->heredoc_fd);
			cmd_list->heredoc_fd = -1;
		}
		cmd_list = cmd_list->next;
	}
}

void	close_fd(t_exec_data *data)
{
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
}

int	heredoc_handle(t_parser *cmd_list, t_exec_data *data, t_env *env_list)
{
	t_parser	*current_cmd;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		current_cmd->heredoc_fd = process_heredocs(current_cmd, env_list);
		if (current_cmd->heredoc_fd == -1)
		{
			set_last_exit_status(130);
			heredoc_fd_error(cmd_list, data, current_cmd);
			write(STDOUT_FILENO, "", 0);
			setup_interactive_signals();
			return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}

int	heredoc_fd_error(t_parser *cmd_list, t_exec_data *data,
		t_parser *current_cmd)
{
	t_parser	*tmp_cmd;

	if (data->original_stdin != -1)
	{
		dup2(data->original_stdin, STDIN_FILENO);
		close(data->original_stdin);
	}
	if (data->original_stdout != -1)
	{
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
	}
	tmp_cmd = cmd_list;
	while (tmp_cmd && tmp_cmd != current_cmd->next)
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

int	ft_heredoc(t_parser *cmd)
{
	if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != -2)
	{
		if (dup2(cmd->heredoc_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 cmd->heredoc_fd after output redir");
			close(cmd->heredoc_fd);
			cmd->heredoc_fd = -1;
			return (1);
		}
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	return (0);
}
