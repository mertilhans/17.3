/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:50 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:28:10 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"


int	is_builtin(t_parser *cmd)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (ft_strcmp(cmd->argv[0], "exit") == 0 || ft_strcmp(cmd->argv[0], "cd") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "pwd") == 0 || ft_strcmp(cmd->argv[0], "echo") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "export") == 0 || ft_strcmp(cmd->argv[0],
			"unset") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (1);
	return 0;
}

int	ft_builtin_call(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	result;

	if (ft_strcmp(cmd->argv[0], "echo") == 0)
	{
		built_echo(cmd);
		return get_last_exit_status();
	}
       if (ft_strcmp(cmd->argv[0], "cd") == 0)
       {
	       t_export **export_list = get_export_list();
	       result = built_cd(cmd, env_list, export_list);
	       return result;
       }
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
	{
		cmd->fd_i = data->original_stdin;
		cmd->fd_o = data->original_stdout;
		builtin_exit(cmd);
		return 0;
	}
	result = ft_builtin_call_2(cmd, env_list);
	if (result != -1)
		return result;
	return -1;
}

int	ft_builtin_call_2(t_parser *cmd, t_env **env_list)
{
	int	result;

	if (ft_strcmp(cmd->argv[0], "export") == 0)
	{
		builtin_export(cmd, env_list);
		return get_last_exit_status();
	}
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
	{
		builtin_pwd();
		return get_last_exit_status();
	}
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
	{
		builtin_unset(cmd, env_list);
		return get_last_exit_status();
	}
	result = ft_builtin_call_3(cmd, env_list);
	if (result != -1)
		return result;
	return -1;
}

int	ft_builtin_call_3(t_parser *cmd, t_env **env_list)
{
	if (ft_strcmp(cmd->argv[0], "env") == 0)
	{
		if (cmd->argv[1])
		{
			ft_putendl_fd(" : No such file or directory\n", 2);
			set_last_exit_status(127);
			return 127;
		}
		builtin_env(*env_list);
		return 0;
	}
	return -1;
}

int	n_next_or_built(t_parser *cmd_list, t_exec_data *data, t_env **env_list)
{
	setup_interactive_signals();
	if (cmd_list->heredoc_fd != -1 && cmd_list->heredoc_fd != -2)
	{
		if (dup2(cmd_list->heredoc_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 heredoc for builtin");
			set_last_exit_status(1);
		}
		close(cmd_list->heredoc_fd);
		cmd_list->heredoc_fd = -1;
	}
	if (ft_redir_ctrl(cmd_list) != 0)
		set_last_exit_status(1);
	ft_builtin_call(cmd_list, data, env_list);
	ft_in_or_out(data);
	setup_interactive_signals();
	return (get_last_exit_status());
}