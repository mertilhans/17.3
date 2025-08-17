/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_main.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:10 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:28:14 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <sys/wait.h>

int	execute_cmds(t_parser *cmd_list, char **env, t_env **env_list)
{
	t_exec_data	data;
	int			pid_len;

	if (data_init(&data, cmd_list, env, &pid_len))
		return (1);
	if (heredoc_handle(cmd_list, &data, *env_list))
	{
		ft_in_or_out(&data);
		setup_interactive_signals();
		if (heredoc_handle_init(cmd_list))
			return (get_last_exit_status());
	}
	setup_interactive_signals();
	return (execute_cmds_2(cmd_list, &data, env_list, &pid_len));
}

int	execute_cmds_2(t_parser *cmd_list, t_exec_data *data, t_env **env_list,
		int *pid_len)
{
	if (*pid_len == 1 && is_builtin(cmd_list))
	{
		set_last_exit_status(n_next_or_built(cmd_list, data, env_list));
		return (get_last_exit_status());
	}
	setup_parent_execution_signals();
	if (ft_data_pids(cmd_list, data, env_list))
	{
		setup_interactive_signals();
		return (1);
	}
	setup_interactive_signals();
	finish_fd(cmd_list, data);
	return (get_last_exit_status());
}

void	execute_loop(t_parser *cmd_list, t_exec_data *data, t_env **env_list)
{
	t_parser	*cmd;

	data->in_fd = STDIN_FILENO;
	cmd = cmd_list;
	while (cmd)
	{
		if (!process_command(cmd, data, env_list))
			break ;
		cmd = cmd->next;
	}
	if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
}

int	process_command(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	data->pipefd[0] = -1;
	data->pipefd[1] = -1;
	if (setup_and_fork(cmd, data, env_list) == -1)
		return 0;
	if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != -2)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
	if (cmd->next)
	{
		if (data->pipefd[0] != -1)
		{
			close(data->pipefd[1]);
			data->in_fd = data->pipefd[0];
		}
		else
			data->in_fd = STDIN_FILENO;
	}
	else
		data->in_fd = STDIN_FILENO;
	return 1;
}

void	wait_pids(t_exec_data *data)
{
	int	status;
	int	j;

	j = 0;
	while (j < data->i)
	{
		waitpid(data->pids[j], &status, 0);
		if (j == data->i - 1)
			set_last_exit_status(calculate_exit_status(status));
		j++;
	}
}