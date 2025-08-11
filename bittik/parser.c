#include "shell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

void ft_clean_init(t_parser *cmd)
{
    cmd->argv_cap = 1;
    cmd->argv = gb_malloc(sizeof(char*) * cmd->argv_cap);
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

void ft_temp_next(t_redirection *new_redir,t_parser *cmd)
{
    t_redirection *temp;

    temp = cmd->redirs;
    while (temp->next)
        temp = temp->next;
    temp->next = new_redir;

}
void add_redirection(t_parser *cmd, t_redir_type type, char *filename)
{
    t_redirection *new_redir;
    
    new_redir = gb_malloc(sizeof(t_redirection));
    if (!new_redir)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    new_redir->filename = ft_strdup(filename); 
    if (!new_redir->filename)
    {
        perror("strdup failed for redirection filename");
        gc_free(new_redir);
        exit(EXIT_FAILURE);
    }
    new_redir->type = type;
    new_redir->no_expand = 0; 
    new_redir->next = NULL;
    if (!cmd->redirs)
        cmd->redirs = new_redir;
    else
        ft_temp_next(new_redir,cmd);
}

void add_redirection_with_expansion(t_parser *cmd, t_redir_type type, char *filename, int no_expand)
{
    t_redirection *new_redir;

    new_redir = gb_malloc(sizeof(t_redirection));
    if (!new_redir)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
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
    if (!cmd->redirs)
        cmd->redirs = new_redir;
    else
        ft_temp_next(new_redir,cmd);
}


void ft_redir_in(t_token *tokens, t_parser *cmd, t_all *all)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
    {
        char *expanded = expand_with_quotes(tokens->value, all->env_list); // exit_status parametresini kaldır
        if (expanded)
            add_redirection(cmd, REDIR_IN, expanded);
        else
            add_redirection(cmd, REDIR_IN, tokens->value); 
        if (expanded && expanded != tokens->value)
            gc_free(expanded);
    }
    else
    {
        cmd->parse_error = 1;
        set_last_exit_status(2);
    }
}

void ft_redir_out(t_token *tokens, t_parser *cmd, t_all *all)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
    {
        char *expanded = expand_with_quotes(tokens->value, all->env_list); // exit_status parametresini kaldır
        if (expanded)
            add_redirection(cmd, REDIR_OUT, expanded);
        else
            add_redirection(cmd, REDIR_OUT, tokens->value);
        if (expanded && expanded != tokens->value)
            gc_free(expanded);
    }
    else
    {
        cmd->parse_error = 1;
        set_last_exit_status(2);
    }
}

void ft_redir_append(t_token *tokens, t_parser *cmd, t_all *all)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
    {
        char *expanded = expand_with_quotes(tokens->value, all->env_list); // exit_status parametresini kaldır
        if (expanded)
            add_redirection(cmd, REDIR_APPEND, expanded);
        else
            add_redirection(cmd, REDIR_APPEND, tokens->value);
        if (expanded && expanded != tokens->value)
            gc_free(expanded);
    }
    else
    {
        cmd->parse_error = 1;
        set_last_exit_status(2);
    }
}

void ft_redir_heredoc(t_token *tokens, t_parser *cmd)
{ 
    char *delimiter;
    char *clean_delimiter;

    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
    {
        delimiter = tokens->value;
        
        if ((delimiter[0] == '"' && delimiter[ft_strlen(delimiter) - 1] == '"') ||
            (delimiter[0] == '\'' && delimiter[ft_strlen(delimiter) - 1] == '\''))
        {
            clean_delimiter = ft_strndup(delimiter + 1, ft_strlen(delimiter) - 2);
            add_redirection_with_expansion(cmd, REDIR_HEREDOC, clean_delimiter, 1);
            gc_free(clean_delimiter);
        }
        else
            add_redirection_with_expansion(cmd, REDIR_HEREDOC, delimiter, 0);
    }
    else
    {
        cmd->parse_error = 1;
        set_last_exit_status(2); // Syntax error
    }
}



void ft_loop_3(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
    int i;
    char **new_argv;
    char *original_token;
    char *expanded;
    int is_unquoted_empty_expansion;
    int has_dollar_expansion;
    int is_quoted;
    char **split_args;
    int j;

    i = 0;
    if ((*tokens)->type == TOKEN_WORD)
    {
        // Argv kapasitesini kontrol et ve gerekirse genişlet
        if (*argc + 1 >= cmd->argv_cap)
        {
            cmd->argv_cap *= 2;
            new_argv = gb_malloc(sizeof(char*) * cmd->argv_cap);
            if (!new_argv)
            {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
            while(i < *argc)
            {
                new_argv[i] = cmd->argv[i];
                i++;
            }
            gc_free(cmd->argv);
            cmd->argv = new_argv;
        }

        original_token = (*tokens)->value;
        expanded = expand_with_quotes(original_token, all->env_list); // exit_status parametresini kaldır
        is_unquoted_empty_expansion = 0;

        // Boş genişletme kontrolü
        if (expanded && expanded[0] == '\0')
        {
            if (!ft_strchr(original_token, '"') && !ft_strchr(original_token, '\''))
            {
                if (ft_strlen(original_token) > 1 && original_token[0] == '$' &&
                    (isalnum(original_token[1]) || original_token[1] == '_'))
                {
                    is_unquoted_empty_expansion = 1;
                }
            }
        }
        
        // İlk argümansa ve boş genişletmeyse çık
        if (*argc == 0 && is_unquoted_empty_expansion)
        {
            if (expanded)
                gc_free(expanded);
            return;
        }

        has_dollar_expansion = ft_strchr(original_token, '$') != NULL;
        is_quoted = ft_strchr(original_token, '"') || ft_strchr(original_token, '\'');

        // Dollar genişletmesi varsa ve quoted değilse ve whitespace içeriyorsa böl
        if (has_dollar_expansion && expanded && !is_quoted &&
            (ft_strchr(expanded, ' ') || ft_strchr(expanded, '\t')))
        {
            split_args = split_expanded_string(expanded);
            j = 0;
            while (split_args && split_args[j])
            {
                // Her yeni argüman için argv kapasitesini kontrol et
                if (*argc + 1 >= cmd->argv_cap)
                {
                    cmd->argv_cap *= 2;
                    new_argv = gb_malloc(sizeof(char*) * cmd->argv_cap);
                    if (!new_argv)
                    {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                    }
                    i = 0;
                    while(i < *argc)
                    {
                        new_argv[i] = cmd->argv[i];
                        i++;
                    }
                    gc_free(cmd->argv);
                    cmd->argv = new_argv;
                }
                
                cmd->argv[*argc] = ft_strdup(split_args[j]);
                if (!cmd->argv[*argc])
                {
                    perror("strdup failed for split arg");
                    exit(EXIT_FAILURE);
                }
                (*argc)++;
                j++;
            }
            
            // split_args'ı temizle
            if (split_args)
            {
                j = 0;
                while (split_args[j])
                {
                    gc_free(split_args[j]);
                    j++;
                }
                gc_free(split_args);
            }
            if (expanded)
                gc_free(expanded);
        }
        else
        {
            // Normal durum - tek argüman ekle
            if (expanded)
            {
                cmd->argv[*argc] = expanded;
            }
            else
            {
                cmd->argv[*argc] = ft_strdup((*tokens)->value);
                if (!cmd->argv[*argc])
                {
                    perror("strdup failed for argv element");
                    exit(EXIT_FAILURE);
                }
            }
            (*argc)++;
        }
    }
}

void ft_loop_2(t_token **tokens, t_parser *cmd,int *argc,t_all *all)
{
    if ((*tokens)->type == TOKEN_REDIR_APPEND)
    {
        ft_redir_append(*tokens, cmd, all);
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
        ft_loop_3(tokens,cmd,argc,all);

}

void ft_loop(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
    if (cmd->parse_error)
    {
        printf("bash: syntax error near unexpected token `newline'\n");
        set_last_exit_status(2);
        return;
    }

    if ((*tokens)->type == TOKEN_REDIR_IN)
    {
        ft_redir_in(*tokens, cmd, all);
        if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
            *tokens = (*tokens)->next;
    }
    else if ((*tokens)->type == TOKEN_REDIR_OUT)
    {
        ft_redir_out(*tokens, cmd, all);
        if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
            *tokens = (*tokens)->next;
    }
    else
        ft_loop_2(tokens, cmd, argc, all);
    
    *tokens = (*tokens)->next;
}

t_token *ft_control_token(t_token *tokens, t_all *data)
{
    int argc;
    t_parser *cmd;

    argc = 0;
    cmd = gb_malloc(sizeof(t_parser));
    if (!cmd)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    ft_clean_init(cmd);
    while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
        ft_loop(&tokens, cmd, &argc, data);
    cmd->argv[argc] = NULL;
    if (!(data->cmd_list))
        data->cmd_list = cmd;
    else
        (data->last_cmd)->next = cmd;
    data->last_cmd = cmd;
    if (tokens && tokens->type == TOKEN_PIPE)
        tokens = tokens->next;
    return tokens;
}

int token_ctrl(t_token *tokens)
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

t_all *ft_all_init(t_all *all,t_env *env) // cmd tanımlı ama kullanılmamış diyor onu düzelttim hata olabilri
{
    all->cmd_list = NULL;
    all->last_cmd = NULL;
    all->env_list = env;
    return (all);
}

t_parser *parse_tokens(t_token *tokens, t_env *env_list)
{
    t_all *all;
    t_parser *cmd;
    
    cmd = gb_malloc(sizeof(t_parser));
    all = gb_malloc(sizeof(t_all));
    all = ft_all_init(all, env_list);
    
    if (token_ctrl(tokens) && !tokens->next)
    {
        printf("bash: syntax error near unexpected token `newline'\n");
        set_last_exit_status(2);
        return NULL;
    }
    while (tokens && tokens->type != TOKEN_EOF)
    {
        while (tokens && tokens->type == TOKEN_PIPE)
        {
            printf("bash: syntax error near unexpected token `|'\n");
            set_last_exit_status(2);
            tokens = tokens->next;
        }
        if (!tokens || tokens->type == TOKEN_EOF)
            break;
        tokens = ft_control_token(tokens, all);  
        if (all->cmd_list && all->cmd_list->parse_error)
        {
            set_last_exit_status(2);
            return NULL;
        }
    }
    cmd = all->cmd_list;
    return cmd;
}
