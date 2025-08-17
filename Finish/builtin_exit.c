/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:31 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:48:32 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	exit_numeric_error(t_parser *cmd)
{
	printf("exit\n");
	printf("bash: exit: %s: numeric argument required\n", cmd->argv[1]);
	close(cmd->fd_i);
	close(cmd->fd_o);
	close_heredoc_fd(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(2);
}

int	is_numeric_string(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	if (str[0] == '+' || str[0] == '-')
		i = 1;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

void	builtin_exit(t_parser *cmd)
{
	int	exit_code;

	exit_code = 0;
	if (cmd->argv[1] && cmd->argv[2])
	{
		printf("bash: exit: too many arguments\n");
		set_last_exit_status(1);
		return ;
	}
	if (cmd->argv[1])
	{
		if (!is_numeric_string(cmd->argv[1]))
			exit_numeric_error(cmd);
		exit_code = ft_atoi(cmd->argv[1]);
		exit_code = ((exit_code % 256) + 256) % 256;
	}
	printf("exit\n");
	close(cmd->fd_i);
	close(cmd->fd_o);
	close_heredoc_fd(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(exit_code);
}
