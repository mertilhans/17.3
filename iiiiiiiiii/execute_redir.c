#include "shell.h"

static int	redir_in(t_redirection *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd < 0)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	redir_out(t_redirection *redir)
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

static int	redir_append(t_redirection *redir)
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

static int	handle_heredoc_fd(t_parser *cmd)
{
	if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != -2)
	{
		if (dup2(cmd->heredoc_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 cmd->heredoc_fd after output redir");
			close(cmd->heredoc_fd);
			cmd->heredoc_fd = -1;
			return (1);
		}
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	return (0);
}

int	setup_file_redirections(t_parser *cmd)
{
	t_redirection	*current_redir;

	current_redir = cmd->redirs;
	while (current_redir)
	{
		if (current_redir->type == REDIR_IN)
		{
			if (redir_in(current_redir) != 0)
				return (-1);
		}
		else if (current_redir->type == REDIR_OUT)
		{
			if (redir_out(current_redir) != 0)
				return (-1);
		}
		else if (current_redir->type == REDIR_APPEND)
		{
			if (redir_append(current_redir) != 0)
				return (-1);
		}
		else if (current_redir->type == REDIR_HEREDOC)
		{
			if (handle_heredoc_fd(cmd))
				return (-1);
		}
		current_redir = current_redir->next;
	}
	return (handle_heredoc_fd(cmd) ? -1 : 0);
}