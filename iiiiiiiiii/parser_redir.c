#include "shell.h"

static void	init_redirection(t_redirection *new_redir, t_redir_type type,
	char *filename, int no_expand)
{
	new_redir->filename = ft_strdup(filename);
	if (!new_redir->filename)
	{
		perror("strdup failed for redirection filename");
		gc_free(new_redir);
		exit(EXIT_FAILURE);
	}
	new_redir->type = type;
	new_redir->no_expand = no_expand;
	new_redir->next = NULL;
}

static void	ft_temp_next(t_redirection *new_redir, t_parser *cmd)
{
	t_redirection	*temp;

	temp = cmd->redirs;
	while (temp->next)
		temp = temp->next;
	temp->next = new_redir;
}

void	add_redirection(t_parser *cmd, t_redir_type type, char *filename)
{
	t_redirection	*new_redir;

	new_redir = gb_malloc(sizeof(t_redirection));
	if (!new_redir)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	init_redirection(new_redir, type, filename, 0);
	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
		ft_temp_next(new_redir, cmd);
}

void	add_redirection_with_expansion(t_parser *cmd, t_redir_type type,
	char *filename, int no_expand)
{
	t_redirection	*new_redir;

	new_redir = gb_malloc(sizeof(t_redirection));
	if (!new_redir)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	init_redirection(new_redir, type, filename, no_expand);
	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
		ft_temp_next(new_redir, cmd);
}

void	process_redirection_expansion(t_token *tokens, t_parser *cmd,
	t_all *all, t_redir_type type)
{
	char	*expanded;

	tokens = tokens->next;
	if (tokens && tokens->type == TOKEN_WORD)
	{
		expanded = expand_with_quotes(tokens->value, all->env_list);
		if (expanded)
			add_redirection(cmd, type, expanded);
		else
			add_redirection(cmd, type, tokens->value);
		if (expanded && expanded != tokens->value)
			gc_free(expanded);
	}
	else
	{
		cmd->parse_error = 1;
		set_last_exit_status(2);
	}
}