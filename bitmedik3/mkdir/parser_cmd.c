#include "shell.h"

void	init_parser_cmd(t_parser *cmd)
{
	if (!cmd)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	cmd->argv_cap = 1;
	cmd->argv = gb_malloc(sizeof(char *) * cmd->argv_cap);
	if (!cmd->argv)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	cmd->redirs = NULL;
	cmd->next = NULL;
	cmd->parse_error = 0;
	cmd->heredoc_fd = -2;
}

static void	process_word_token(t_token **tokens, t_parser *cmd,
							int *argc, t_all *all)
{
	char	*original_token;
	char	*expanded;

	if ((*tokens)->type != TOKEN_WORD)
		return ;
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

static void	process_token(t_token **tokens, t_parser *cmd,
						int *argc, t_all *all)
{
	if (cmd->parse_error)
	{
		printf("bash: syntax error near unexpected token `newline'\n");
		set_last_exit_status(2);
		return ;
	}
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
	else
		process_word_token(tokens, cmd, argc, all);
	*tokens = (*tokens)->next;
}

t_token	*ft_control_token(t_token *tokens, t_all *data)
{
	int			argc;
	t_parser	*cmd;

	argc = 0;
	cmd = gb_malloc(sizeof(t_parser));
	init_parser_cmd(cmd);
	while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
		process_token(&tokens, cmd, &argc, data);
	cmd->argv[argc] = NULL;
	if (!(data->cmd_list))
		data->cmd_list = cmd;
	else
		(data->last_cmd)->next = cmd;
	data->last_cmd = cmd;
	if (tokens && tokens->type == TOKEN_PIPE)
		tokens = tokens->next;
	return (tokens);
}
