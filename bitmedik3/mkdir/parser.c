#include "shell.h"

static int	check_token_syntax_pipe(t_token *current)
{
	// Pipe başta veya arka arkaya ise hata ver
	if (current->type == TOKEN_PIPE && (!current->next || current->next->type == TOKEN_PIPE || current->next->type == TOKEN_EOF))
	{
		if (!current->next || current->next->type == TOKEN_EOF)
			printf("bash: syntax error near unexpected token `newline'\n");
		else
			printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}

static int	check_token_syntax_redir(t_token *current)
{
	if (current->type == TOKEN_REDIR_IN
		|| current->type == TOKEN_REDIR_OUT
		|| current->type == TOKEN_REDIR_APPEND
		|| current->type == TOKEN_HEREDOC)
	{
		if (!current->next || current->next->type != TOKEN_WORD)
		{
			printf("bash: syntax error near unexpected token `newline'\n");
			set_last_exit_status(2);
			return (1);
		}
	}
	return (0);
}

int	check_token_syntax(t_token *tokens)
{
	t_token *current;

	current = tokens;
	if (!tokens)
		return (0);
	if (tokens->type == TOKEN_PIPE) // başta pipe varsa
	{
		printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	while (current)
	{
		if (check_token_syntax_redir(current))
			return (1);
		if (check_token_syntax_pipe(current))
			return (1);
		current = current->next;
	}
	return (0);
}

static int	token_ctrl(t_token *tokens)
{
	if (tokens->type == TOKEN_REDIR_IN)
		return (1);
	if (tokens->type == TOKEN_REDIR_OUT)
		return (1);
	if (tokens->type == TOKEN_REDIR_APPEND)
		return (1);
	if (tokens->type == TOKEN_HEREDOC)
		return (1);
	return (0);
}

t_parser	*parse_tokens(t_token *tokens, t_env *env_list)
{
	t_all		*all;
	t_parser	*cmd;

	cmd = gb_malloc(sizeof(t_parser));
	all = gb_malloc(sizeof(t_all));
	all->cmd_list = NULL;
	all->last_cmd = NULL;
	all->env_list = env_list;
	if (check_token_syntax(tokens))
		return (NULL);
	if (token_ctrl(tokens) && !tokens->next)
	{
		printf("bash: syntax error near unexpected token `newline'\n");
		set_last_exit_status(2);
		return (NULL);
	}
	while (tokens && tokens->type != TOKEN_EOF)
	{
		tokens = ft_control_token(tokens, all);
		if (all->cmd_list && all->cmd_list->parse_error)
		{
			set_last_exit_status(2);
			return (NULL);
		}
	}
	cmd = all->cmd_list;
	return (cmd);
}