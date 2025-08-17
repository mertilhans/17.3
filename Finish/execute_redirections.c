/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:12 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 17:28:11 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <fcntl.h>

int	ft_redir_ctrl(t_parser *cmd)
{
	t_redirection	*redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == REDIR_IN)
		{
			if (redir_in(redir) != 0)
				return (-1);
		}
		else if (redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
		{
			if (ft_redir_in_or_out(redir))
				return (-1);
		}
		else if (redir->type == REDIR_HEREDOC)
		{
			if (ft_heredoc(cmd))
				return (-1);
		}
		redir = redir->next;
	}
	if (ft_heredoc(cmd))
		return (-1);
	return (0);
}

int	redir_in(t_redirection *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd < 0)
	{
		perror(redir->filename);
		return 1;
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return 0;
}

int	redir_out(t_redirection *redir)
{
	int	fd;

	fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	redir_append(t_redirection *redir)
{
	int	fd;

	fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (fd < 0)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	ft_redir_in_or_out(t_redirection *redir)
{
	if (redir->type == REDIR_OUT)
	{
		if (redir_out(redir) != 0)
			return (1);
	}
	else
	{
		if (redir_append(redir) != 0)
			return (1);
	}
	return (0);
}