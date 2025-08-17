#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

void    ft_clean_init(t_parser *cmd)
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
        cmd->heredoc_fd = -2;
}

void    ft_temp_next(t_redirection *new_redir, t_parser *cmd)
{
        t_redirection   *temp;

        temp = cmd->redirs;
        while (temp->next)
                temp = temp->next;
        temp->next = new_redir;
}

void    init_redirection(t_redirection *new_redir, t_redir_type type,
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

void    init_parser_cmd(t_parser *cmd)
{
        if (!cmd)
        {
                perror("malloc failed");
                exit(EXIT_FAILURE);
        }
        ft_clean_init(cmd);
}

void    finalize_cmd(t_parser *cmd, int argc, t_all *data)
{
        cmd->argv[argc] = NULL;
        if (!(data->cmd_list))
                data->cmd_list = cmd;
        else
                (data->last_cmd)->next = cmd;
        data->last_cmd = cmd;
}