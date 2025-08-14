#include "shell.h"


t_parser	*parse_token_list(t_token *tokens, t_all *all);

static void	ft_clean_init(t_parser *cmd)
{
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

void	init_parser_cmd(t_parser *cmd)
{
	if (!cmd)
	{
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	ft_clean_init(cmd);
}

static t_all	*ft_all_init(t_all *all, t_env *env)
{
	all->cmd_list = NULL;
	all->last_cmd = NULL;
	all->env_list = env;
	return (all);
}

static int	ft_syn(t_token *tokens)
{
	t_token	*token;

	token = tokens;
	while (token)
	{
		if (token->type == TOKEN_REDIR_OUT
			|| token->type == TOKEN_REDIR_APPEND
			|| token->type == TOKEN_REDIR_IN
			|| token->type == TOKEN_HEREDOC)
		{
			if (!token->next)
			{
				printf("bash: syntax error near unexpected token `newline'\n");
				return (1);
			}
		}
		token = token->next;
	}
	return (0);
}

t_parser	*parse_tokens(t_token *tokens, t_env *env_list)
{
	t_all	*all;

	all = gb_malloc(sizeof(t_all));
	all = ft_all_init(all, env_list);
	if (ft_syn(tokens))
		return (NULL);
	return (parse_token_list(tokens, all));
}