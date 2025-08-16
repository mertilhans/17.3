#include "shell.h"

void    handle_split_expansion(t_parser *cmd, int *argc, char *original_token,
                char *expanded)
{
        char    **split_args;

        (void)original_token;
        split_args = split_expanded_string(expanded);
        add_split_args(cmd, argc, split_args);
        cleanup_split_args(split_args);
        if (expanded)
                gc_free(expanded);
}

void    handle_normal_expansion(t_parser *cmd, int *argc, t_token *token,
                char *expanded)
{
        if (expanded)
                cmd->argv[*argc] = expanded;
        else
        {
                cmd->argv[*argc] = ft_strdup(token->value);
                if (!cmd->argv[*argc])
                {
                        perror("strdup failed for argv element");
                        exit(EXIT_FAILURE);
                }
        }
        (*argc)++;
}

void    ft_loop_3(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
        char    *original_token;
        char    *expanded;

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

void    ft_loop_2(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
        if ((*tokens)->type == TOKEN_REDIR_APPEND)
        {
                redir_control_state(*tokens, cmd, all, REDIR_APPEND);
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
                ft_loop_3(tokens, cmd, argc, all);
}

void    ft_loop(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
        if ((*tokens)->type == TOKEN_REDIR_IN)
        {
                redir_control_state(*tokens, cmd, all, REDIR_IN);
                if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
                        *tokens = (*tokens)->next;
        }
        else if ((*tokens)->type == TOKEN_REDIR_OUT)
        {
                redir_control_state(*tokens, cmd, all, REDIR_OUT);
                if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
                        *tokens = (*tokens)->next;
        }
        else
                ft_loop_2(tokens, cmd, argc, all);
        *tokens = (*tokens)->next;
}