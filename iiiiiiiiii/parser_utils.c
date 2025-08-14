#include "shell.h"

/* Helper function declarations */
int		has_quote_chars(char *delimiter);
char	*remove_quotes(char *delimiter);

static void	ft_redir_heredoc(t_token *tokens, t_parser *cmd)
{
	char	*delimiter;
	char	*clean_delimiter;
	int		quotes_found;

	tokens = tokens->next;
	if (tokens && tokens->type == TOKEN_WORD)
	{
		delimiter = tokens->value;
		quotes_found = has_quote_chars(delimiter);
		clean_delimiter = remove_quotes(delimiter);
		if (!clean_delimiter)
		{
			cmd->parse_error = 1;
			return ;
		}
		if (quotes_found)
			add_redirection_with_expansion(cmd, REDIR_HEREDOC,
				clean_delimiter, 1);
		else
			add_redirection_with_expansion(cmd, REDIR_HEREDOC,
				clean_delimiter, 0);
		gc_free(clean_delimiter);
	}
	else
	{
		cmd->parse_error = 1;
		set_last_exit_status(2);
	}
}

static void	process_word_token(t_token **tokens, t_parser *cmd,
	int *argc, t_all *all)
{
	char	*original_token;
	char	*expanded;

	if (*argc + 1 >= cmd->argv_cap)
		expand_argv_capacity(cmd, argc);
	original_token = (*tokens)->value;
	expanded = expand_with_quotes(original_token, all->env_list);
       if (*argc == 0 && is_empty_expansion(original_token, expanded))
       {
	       if (expanded)
		       gc_free(expanded);
	       return ;
       }
       if (should_split_expansion(original_token, expanded))
	       handle_split_expansion(cmd, argc, original_token, expanded);
       else
	       handle_normal_expansion(cmd, argc, *tokens, expanded);
}

static void	handle_redirections(t_token **tokens, t_parser *cmd, t_all *all)
{
	if ((*tokens)->type == TOKEN_REDIR_IN)
	{
		process_redirection_expansion(*tokens, cmd, all, REDIR_IN);
		if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
			*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == TOKEN_REDIR_OUT)
	{
		process_redirection_expansion(*tokens, cmd, all, REDIR_OUT);
		if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
			*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == TOKEN_REDIR_APPEND)
	{
		process_redirection_expansion(*tokens, cmd, all, REDIR_APPEND);
		if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
			*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == TOKEN_HEREDOC)
	{
		ft_redir_heredoc(*tokens, cmd);
		if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
			*tokens = (*tokens)->next;
	}
}

static t_token	*ft_control_token(t_token *tokens, t_all *data)
{
	int			argc;
	t_parser	*cmd;

	argc = 0;
	cmd = gb_malloc(sizeof(t_parser));
	init_parser_cmd(cmd);
	while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
	{
		if (cmd->parse_error)
		{
			printf("bash: syntax error near unexpected token `newline'\n");
			set_last_exit_status(2);
			return (tokens);
		}
		if (tokens->type == TOKEN_WORD)
			process_word_token(&tokens, cmd, &argc, data);
		else
			handle_redirections(&tokens, cmd, data);
		tokens = tokens->next;
	}
	/* finalize_cmd inline here */
	cmd->argv[argc] = NULL;
	if (!(data->cmd_list))
		data->cmd_list = cmd;
	else
		(data->last_cmd)->next = cmd;
	data->last_cmd = cmd;
	/* end finalize_cmd */
	if (tokens && tokens->type == TOKEN_PIPE)
		tokens = tokens->next;
	return (tokens);
}

t_parser	*parse_token_list(t_token *tokens, t_all *all)
{
	while (tokens && tokens->type != TOKEN_EOF)
	{
		while (tokens && tokens->type == TOKEN_PIPE)
		{
			printf("bash: syntax error near unexpected token `|'\n");
			set_last_exit_status(2);
			tokens = tokens->next;
		}
		if (!tokens || tokens->type == TOKEN_EOF)
			break ;
		tokens = ft_control_token(tokens, all);
		if (all->cmd_list && all->cmd_list->parse_error)
		{
			set_last_exit_status(2);
			return (NULL);
		}
	}
	return (all->cmd_list);
}