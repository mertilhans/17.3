#include "shell.h"

int	process_heredocs(t_parser *cmd, t_env *env_list)
{
	t_heredoc_data	h_data;
	t_redirection	*current_redir;

	h_data.heredoc_content = NULL;
	h_data.heredoc_len = 0;
	h_data.last_heredoc_fd = -2;
	current_redir = cmd->redirs;
	while (current_redir)
	{
		if (current_redir->type == REDIR_HEREDOC)
		{
			if (h_data.last_heredoc_fd != -2 && h_data.last_heredoc_fd != -1)
			{
				close(h_data.last_heredoc_fd);
				h_data.last_heredoc_fd = -1;
			}
			if (ft_h_built_expand(current_redir, &h_data, env_list) == -1)
				return (-1);
		}
		current_redir = current_redir->next;
	}
	return (h_data.last_heredoc_fd);
}