/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_fd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:59 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/25 16:27:23 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

void	cleanup_heredoc_fds(t_parser *start_cmd, t_parser *end_cmd)
{
	t_parser	*temp;

	temp = start_cmd;
	while (temp != end_cmd)
	{
		if (temp->heredoc_fd > 2)
			close(temp->heredoc_fd);
		if (temp->heredoc_fd > 2)
			temp->heredoc_fd = -1;
		temp = temp->next;
	}
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
			cleanup_heredoc_fds(cmd_list, current_cmd);
			set_last_exit_status(130);
			ft_in_or_out(data);
			setup_interactive_signals();
			return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0);
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
