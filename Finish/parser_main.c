#include "shell.h"

t_token *ft_control_token(t_token *tokens, t_all *data)
{
        int                     argc;
        t_parser        *cmd;

        argc = 0;
        cmd = gb_malloc(sizeof(t_parser));
        init_parser_cmd(cmd);
        while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
                ft_loop(&tokens, cmd, &argc, data);
        finalize_cmd(cmd, argc, data);
        if (tokens && tokens->type == TOKEN_PIPE)
                tokens = tokens->next;
        return (tokens);
}

int     token_ctrl(t_token *tokens)
{
        if (tokens->type == TOKEN_REDIR_IN)
                return (1);
        if (tokens->type == TOKEN_REDIR_OUT)
                return (1);
        if (tokens->type == TOKEN_REDIR_APPEND)
                return (1);
        if (tokens->type == TOKEN_HEREDOC)
                return 1;
        return 0;
}

t_all   *ft_all_init(t_all *all, t_env *env)
{
        all->cmd_list = NULL;
        all->last_cmd = NULL;
        all->env_list = env;
        return all;
}

void    handle_pipe_errors(t_token **tokens)
{
        while (*tokens && (*tokens)->type == TOKEN_PIPE)
        {
                ft_putendl_fd("bash: syntax error near unexpected token `|'\n", 2);
                set_last_exit_status(2);
                *tokens = (*tokens)->next;
        }
}

t_parser        *parse_tokens(t_token *tokens, t_env *env_list)
{
        t_all           *all;
        t_parser        *cmd;

        cmd = gb_malloc(sizeof(t_parser));
        all = gb_malloc(sizeof(t_all));
        all = ft_all_init(all, env_list);
        if (ft_syn(tokens))
                return (NULL);
        if (token_ctrl(tokens) && !tokens->next)
        {
                ft_putendl_fd("bash: syntax error near unexpected token `newline'\n",
                                2);
                set_last_exit_status(2);
                return NULL;
        }
        while (tokens && tokens->type != TOKEN_EOF)
        {
                handle_pipe_errors(&tokens);
                if (!tokens || tokens->type == TOKEN_EOF)
                        break ;
                tokens = ft_control_token(tokens, all);
        }
        cmd = all->cmd_list;
        return cmd;
}