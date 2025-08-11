#include "shell.h"

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
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
		current_redir = current_redir->next;
	}
	return (0);
}