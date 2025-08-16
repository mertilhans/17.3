#include "shell.h"

void    add_redirection(t_parser *cmd, t_redir_type type, char *filename)
{
        t_redirection   *new_redir;

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

void    add_redirection_with_expansion(t_parser *cmd, t_redir_type type,
                char *filename, int no_expand)
{
        t_redirection   *new_redir;

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

void    process_redirection_expansion(t_token *tokens, t_parser *cmd,
                t_all *all, t_redir_type type)
{
        char    *expanded;

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
                set_last_exit_status(2);
}

void    redir_control_state(t_token *tokens, t_parser *cmd, t_all *all,
                int result)
{
        if (result == REDIR_IN)
                process_redirection_expansion(tokens, cmd, all, REDIR_IN);
        if (result == REDIR_OUT)
                process_redirection_expansion(tokens, cmd, all, REDIR_OUT);
        if (result == REDIR_APPEND)
                process_redirection_expansion(tokens, cmd, all, REDIR_APPEND);
}

void    ft_redir_heredoc(t_token *tokens, t_parser *cmd)
{
        char    *delimiter;
        char    *clean_delimiter;
        int             quotes_found;

        tokens = tokens->next;
        if (tokens && tokens->type == TOKEN_WORD)
        {
                delimiter = tokens->value;
                quotes_found = has_quote_chars(delimiter);
                clean_delimiter = remove_quotes(delimiter);
                if (!clean_delimiter)
                        return ;
                if (quotes_found)
                        add_redirection_with_expansion(cmd, REDIR_HEREDOC, clean_delimiter,
                                        1);
                else
                        add_redirection_with_expansion(cmd, REDIR_HEREDOC, clean_delimiter,
                                        0);
                gc_free(clean_delimiter);
        }
}
