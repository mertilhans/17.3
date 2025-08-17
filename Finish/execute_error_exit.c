/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_error_exit.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:56 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:34:51 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	ft_directory(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": is a directory\n", 2);
	ft_free_and_close(data,cmd);
	exit(126);
}

void	ft_not_directory(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": No such file or directory\n", 2);
	ft_free_and_close(data,cmd);
	exit(127);
}

void	ft_not_executable(t_parser *cmd, t_exec_data *data, char *exec_path)
{
	if (!exec_path)
	{
		ft_putendl_fd("bash: ", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found\n", 2);
	}
	else
	{
		ft_putendl_fd("bash: ", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd(": Permission denied\n", 2);
	}
	ft_free_and_close(data,cmd);
	exit(127);
}

void	ft_permission_denied(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": Permission denied\n", 2);
	ft_free_and_close(data,cmd);
	exit(126);
}

void	not_her_app_exp(t_heredoc_buffer *buf)
{
	perror("line error");
	gc_free(buf->line);
	gc_free(buf->content);
	setup_interactive_signals();
}