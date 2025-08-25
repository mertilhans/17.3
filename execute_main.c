/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_main.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:10 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/25 19:34:35 by merilhan         ###   ########.fr       */
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
		heredoc_handle_init(cmd_list);
		return (get_last_exit_status());
	}
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
	ft_init_signals(IGNORE); 
	if (ft_data_pids(cmd_list, data, env_list))
	{
		ft_init_signals(PROMPT); 
		return (1);
	}
	finish_fd(cmd_list, data);
	ft_init_signals(PROMPT); 
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
		{
			if (data->in_fd != STDIN_FILENO && data->in_fd > 2)
				close(data->in_fd);
			close_all_heredoc_fds(cmd);
			break ;
		}
		cmd = cmd->next;
	}
	if (data->in_fd != STDIN_FILENO && data->in_fd > 2)
	{
		close(data->in_fd);
		data->in_fd = STDIN_FILENO;
	}
}

int	process_command(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	if (setup_and_fork(cmd, data, env_list))
		return (0);
	if (cmd->heredoc_fd > 2)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	if (data->in_fd != STDIN_FILENO && data->in_fd > 2)
	{
		close(data->in_fd);
		data->in_fd = STDIN_FILENO;
	}
	if (cmd->next)
	{
		close(data->pipefd[1]);
		data->in_fd = data->pipefd[0];
	}
	return (1);
}

void	wait_pids(t_exec_data *data)
{
	int	status;
	int	j;
	int	sig;

	j = 0;
	while (j < data->i)
	{
		waitpid(data->pids[j], &status, 0);
		if (WIFSIGNALED(status))
		{
			sig = WTERMSIG(status);
			if (sig == SIGINT)
				write(STDOUT_FILENO, "\n", 1);
			else if (sig == SIGQUIT)
				write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		}
		if (j == data->i - 1)
			set_last_exit_status(calculate_exit_status(status));
		j++;
	}
}