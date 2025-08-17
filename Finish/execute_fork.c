/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:04 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:05 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	setup_and_fork(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	pid;

	if (setup_pipe_if_needed(cmd, data) == -1)
		return (-1);
	pid = fork_pipe_exec(cmd, data, env_list);
	if (pid == -1)
	{
		if (cmd->next)
		{
			close(data->pipefd[0]);
			close(data->pipefd[1]);
		}
		return -1;
	}
	if (data->i >= data->pids_size)
	{
		printf("limit error\n");
		return (-1);
	}
	data->pids[data->i++] = pid;
	return (pid);
}

int	fork_pipe_exec(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1;
	}
	if (pid == 0)
	{
		child_process_exec(cmd, data, env_list);
		exit(1);
	}
	return (pid);
}

int	setup_pipe_if_needed(t_parser *cmd, t_exec_data *data)
{
	if (!cmd->next)
		return (0);
	if (pipe(data->pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}
	return 0;
}