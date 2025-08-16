#include "shell.h"

int     ft_syn(t_token *tokens)
{
        t_token *token;

        token = tokens;
        while (token)
        {
                if (token->type == TOKEN_REDIR_OUT ||
                        token->type == TOKEN_REDIR_APPEND ||
                        token->type == TOKEN_REDIR_IN ||
                        token->type == TOKEN_HEREDOC)
                {
                        if (!token->next)
                        {
                                ft_putendl_fd("bash: syntax error near unexpected token `newline'\n",
                                                2);
                                return (1);
                        }
                }
                token = token->next;
                void ft_redir_append(t_token * tokens, t_parser * cmd, t_all * all);
        }
        return (0);
}