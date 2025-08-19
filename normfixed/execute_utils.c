/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:15 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 15:41:15 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <unistd.h>

void	ft_putendl_fd(char *str, int fd)
{
	if (!str)
		return ;
	while (*str)
		write(fd, str++, 1);
}

int	count_commands(t_parser *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

int	get_argv_count(char **argv)
{
	int	count;

	count = 0;
	while (argv[count])
		count++;
	return (count);
}

char	*copy_dir(const char *dir, int len)
{
	char	*res;
	int		i;

	i = 0;
	res = gb_malloc(len + 1);
	if (!res)
		return (NULL);
	while (i < len)
	{
		res[i] = dir[i];
		i++;
	}
	res[len] = '\0';
	return (res);
}

void	ft_free_and_close(t_exec_data *data, t_parser *cmd)
{
	close_fd(data);
	close_heredoc_fd(cmd);
	gb_free_all();
	env_gb_free_all();
}
