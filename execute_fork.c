/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:04 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/24 16:11:03 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

void	ft_exec_ctrl(t_parser *cmd, t_exec_data *data, char **exec_path,
		struct stat *path_stat)
{
	*exec_path = NULL;
	if (ft_strchr(cmd->argv[0], '/'))
	{
		if (stat(cmd->argv[0], path_stat) != 0)
			ft_not_directory(cmd, data);
		if (S_ISDIR(path_stat->st_mode))
			ft_directory(cmd, data);
		if (access(cmd->argv[0], X_OK) != 0)
			ft_permission_denied(cmd, data);
		*exec_path = cmd->argv[0];
	}
}

int	setup_and_fork(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	if (cmd->next)
	{
		if (pipe(data->pipefd) == -1)
		{
			perror("pipe");
			return (1);
		}
	}
	else
	{
		data->pipefd[0] = -1;
		data->pipefd[1] = -1;
	}
	if (fork_pipe_exec(cmd, data, env_list) == 0)
		return (1);
	return (0);
}

int	fork_pipe_exec(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (cmd->next)
			close_pipe_fds(data);
		return (0);
	}
	if (pid == 0)
	{
		child_process_exec(cmd, data, env_list);
		exit(1);
	}
	data->pids[data->i++] = pid;
	return (pid);
}

int	setup_pipe_if_needed(t_parser *cmd, t_exec_data *data)
{
	if (!cmd->next)
		return (0);
	if (pipe(data->pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}
