/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:07 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:08 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	data_init(t_exec_data *data, t_parser *cmd_list, char **env, int *pid_len)
{
	data->original_stdin = dup(STDIN_FILENO);
	data->original_stdout = dup(STDOUT_FILENO);
	if (data->original_stdin == -1 || data->original_stdout == -1)
	{
		perror("dup failed");
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		return 1;
	}
	*pid_len = count_commands(cmd_list);
	data->pids_size = *pid_len;
	data->pids = NULL;
	data->i = 0;
	cmd_list->fd_i = data->original_stdin;
	cmd_list->fd_o = data->original_stdout;
	data->env = env;
	data->in_fd = STDIN_FILENO;
	data->pipefd[0] = -1;
	data->pipefd[1] = -1;
	return 0;
}

int	ft_data_pids(t_parser *cmd_list, t_exec_data *data, t_env **env_list)
{
	t_parser	*current_cmd;

	current_cmd = cmd_list;
	data->pids = gb_malloc(sizeof(int) * data->pids_size);
	if (!data->pids)
	{
		perror("pids error\n");
		current_cmd = cmd_list;
		while (current_cmd)
		{
			if (current_cmd->heredoc_fd != -1 && current_cmd->heredoc_fd != -2)
			{
				close(current_cmd->heredoc_fd);
				current_cmd->heredoc_fd = -1;
			}
			current_cmd = current_cmd->next;
		}
		dup2(data->original_stdin, STDIN_FILENO);
		close(data->original_stdin);
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
		return (1);
	}
	exe_loop(cmd_list, data, env_list);
	return (0);
}

void	exe_loop(t_parser *cmd_list, t_exec_data *data, t_env **env_list)
{
	execute_loop(cmd_list, data, env_list);
	wait_pids(data);
	gc_free(data->pids);
}

void	finish_fd(t_parser *cmd_list, t_exec_data *data)
{
	t_parser	*current_cmd;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		if (current_cmd->heredoc_fd != -1 && current_cmd->heredoc_fd != -2)
		{
			close(current_cmd->heredoc_fd);
			current_cmd->heredoc_fd = -1;
		}
		current_cmd = current_cmd->next;
	}
	dup2(data->original_stdin, STDIN_FILENO);
	close(data->original_stdin);
	dup2(data->original_stdout, STDOUT_FILENO);
	close(data->original_stdout);
}

void	ft_in_or_out(t_exec_data *data)
{
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
}