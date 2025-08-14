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

void ft_temp_next(t_redirection *new_redir, t_parser *cmd)
{
    t_redirection *temp;

    temp = cmd->redirs;
    while (temp->next)
        temp = temp->next;
    temp->next = new_redir;
}

void init_redirection(t_redirection *new_redir, t_redir_type type, char *filename, int no_expand)
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

void add_redirection(t_parser *cmd, t_redir_type type, char *filename)
{
    t_redirection *new_redir;
    
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

void add_redirection_with_expansion(t_parser *cmd, t_redir_type type, char *filename, int no_expand)
{
    t_redirection *new_redir;

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

void process_redirection_expansion(t_token *tokens, t_parser *cmd, t_all *all, t_redir_type type)
{
    char *expanded;
    
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
    {
        cmd->parse_error = 1;
        set_last_exit_status(2);
    }
}

void ft_redir_in(t_token *tokens, t_parser *cmd, t_all *all)
{
    process_redirection_expansion(tokens, cmd, all, REDIR_IN);
}

void ft_redir_out(t_token *tokens, t_parser *cmd, t_all *all)
{
    process_redirection_expansion(tokens, cmd, all, REDIR_OUT);
}

void ft_redir_append(t_token *tokens, t_parser *cmd, t_all *all)
{
    process_redirection_expansion(tokens, cmd, all, REDIR_APPEND);
}

int has_quote_chars(char *delimiter)
{
    int i;
    int len;

    i = 0;
    len = ft_strlen(delimiter);
    while (i < len)
    {
        if (delimiter[i] == '"' || delimiter[i] == '\'')
            return 1;
        i++;
    }
    return 0;
}

char *remove_quotes(char *delimiter)
{
    char *clean_delimiter;
    int i;
    int j;
    int len;

    len = ft_strlen(delimiter);
    clean_delimiter = gb_malloc(len + 1);
    if (!clean_delimiter)
        return NULL;
    i = 0;
    j = 0;
    while (i < len)
    {
        if (delimiter[i] != '"' && delimiter[i] != '\'')
            clean_delimiter[j++] = delimiter[i];
        i++;
    }
    clean_delimiter[j] = '\0';
    return clean_delimiter;
}

void ft_redir_heredoc(t_token *tokens, t_parser *cmd)
{ 
    char *delimiter;
    char *clean_delimiter;
    int quotes_found;

    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
    {
        delimiter = tokens->value;
        quotes_found = has_quote_chars(delimiter);
        clean_delimiter = remove_quotes(delimiter);
        if (!clean_delimiter)
        {
            cmd->parse_error = 1;
            return;
        }
        if (quotes_found)
            add_redirection_with_expansion(cmd, REDIR_HEREDOC, clean_delimiter, 1);
        else
            add_redirection_with_expansion(cmd, REDIR_HEREDOC, clean_delimiter, 0);
        gc_free(clean_delimiter);
    }
    else
    {
        cmd->parse_error = 1;
        set_last_exit_status(2);
    }
}

void expand_argv_capacity(t_parser *cmd, int *argc)
{
    char **new_argv;
    int i;

    cmd->argv_cap *= 2;
    new_argv = gb_malloc(sizeof(char*) * cmd->argv_cap);
    if (!new_argv)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    i = 0;
    while (i < *argc)
    {
        new_argv[i] = cmd->argv[i];
        i++;
    }
    gc_free(cmd->argv);
    cmd->argv = new_argv;
}

int is_empty_expansion(char *original_token, char *expanded)
{
    if (!expanded || expanded[0] != '\0')
        return 0;
    if (ft_strchr(original_token, '"') || ft_strchr(original_token, '\''))
        return 0;
    if (ft_strlen(original_token) <= 1 || original_token[0] != '$')
        return 0;
    if (!isalnum(original_token[1]) && original_token[1] != '_')
        return 0;
    return 1;
}

int should_split_expansion(char *original_token, char *expanded)
{
    int has_dollar_expansion;
    int is_quoted;

    has_dollar_expansion = ft_strchr(original_token, '$') != NULL;
    is_quoted = ft_strchr(original_token, '"') || ft_strchr(original_token, '\'');
    if (!has_dollar_expansion || !expanded || is_quoted)
        return 0;
    if (!ft_strchr(expanded, ' ') && !ft_strchr(expanded, '\t'))
        return 0;
    return 1;
}

void add_split_args(t_parser *cmd, int *argc, char **split_args)
{
    int j;

    j = 0;
    while (split_args && split_args[j])
    {
        if (*argc + 1 >= cmd->argv_cap)
            expand_argv_capacity(cmd, argc);
        cmd->argv[*argc] = ft_strdup(split_args[j]);
        if (!cmd->argv[*argc])
        {
            perror("strdup failed for split arg");
            exit(EXIT_FAILURE);
        }
        (*argc)++;
        j++;
    }
}

void cleanup_split_args(char **split_args)
{
    int j;

    if (!split_args)
        return;
    j = 0;
    while (split_args[j])
    {
        gc_free(split_args[j]);
        j++;
    }
    gc_free(split_args);
}

void handle_split_expansion(t_parser *cmd, int *argc, char *original_token, char *expanded)
{
    char **split_args;

    (void)original_token;
    split_args = split_expanded_string(expanded);
    add_split_args(cmd, argc, split_args);
    cleanup_split_args(split_args);
    if (expanded)
        gc_free(expanded);
}

void handle_normal_expansion(t_parser *cmd, int *argc, t_token *token, char *expanded)
{
    if (expanded)
    {
        cmd->argv[*argc] = expanded;
    }
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

void ft_loop_3(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
{
    char *original_token;
    char *expanded;

    if ((*tokens)->type != TOKEN_WORD)
        return;
    if (*argc + 1 >= cmd->argv_cap)
        expand_argv_capacity(cmd, argc);
    original_token = (*tokens)->value;
    expanded = expand_with_quotes(original_token, all->env_list);
    if (*argc == 0 && is_empty_expansion(original_token, expanded))
    {
        if (expanded)
            gc_free(expanded);
        return;
    }
    if (should_split_expansion(original_token, expanded))
        handle_split_expansion(cmd, argc, original_token, expanded);
    else
        handle_normal_expansion(cmd, argc, *tokens, expanded);
}

void ft_loop_2(t_token **tokens, t_parser *cmd, int *argc, t_all *all)
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
        ft_loop_3(tokens, cmd, argc, all);
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

void init_parser_cmd(t_parser *cmd)
{
    if (!cmd)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    ft_clean_init(cmd);
}

void finalize_cmd(t_parser *cmd, int argc, t_all *data)
{
    cmd->argv[argc] = NULL;
    if (!(data->cmd_list))
        data->cmd_list = cmd;
    else
        (data->last_cmd)->next = cmd;
    data->last_cmd = cmd;
}

t_token *ft_control_token(t_token *tokens, t_all *data)
{
    int argc;
    t_parser *cmd;

    argc = 0;
    cmd = gb_malloc(sizeof(t_parser));
    init_parser_cmd(cmd);
    while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
        ft_loop(&tokens, cmd, &argc, data);
    finalize_cmd(cmd, argc, data);
    if (tokens && tokens->type == TOKEN_PIPE)
        tokens = tokens->next;
    return tokens;
}

int token_ctrl(t_token *tokens)
{
    if (tokens->type == TOKEN_REDIR_IN)
        return 1;
    if (tokens->type == TOKEN_REDIR_OUT)
        return 1;
    if (tokens->type == TOKEN_REDIR_APPEND)
        return 1;
    if (tokens->type == TOKEN_HEREDOC)
        return 1;
    return 0;
}

t_all *ft_all_init(t_all *all, t_env *env)
{
    all->cmd_list = NULL;
    all->last_cmd = NULL;
    all->env_list = env;
    return all;
}

void handle_pipe_errors(t_token **tokens)
{
    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        printf("bash: syntax error near unexpected token `|'\n");
        set_last_exit_status(2);
        *tokens = (*tokens)->next;
    }
}

int check_parse_errors(t_all *all)
{
    if (all->cmd_list && all->cmd_list->parse_error)
    {
        set_last_exit_status(2);
        return 1;
    }
    return 0;
}
int ft_syn(t_token *tokens)
{
    t_token *token = tokens;
    while(token)
    {
        if (token->type == TOKEN_REDIR_OUT ||
            token->type == TOKEN_REDIR_APPEND ||
            token->type == TOKEN_REDIR_IN ||
            token->type == TOKEN_HEREDOC)
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

t_parser *parse_tokens(t_token *tokens, t_env *env_list)
{
    t_all *all;
    t_parser *cmd;
    cmd = gb_malloc(sizeof(t_parser));
    all = gb_malloc(sizeof(t_all));
    all = ft_all_init(all, env_list);
    if(ft_syn(tokens))
        return (NULL);
    if (token_ctrl(tokens) && !tokens->next)
    {
        printf("bash: syntax error near unexpected token `newline'\n");
        set_last_exit_status(2);
        return NULL;
    }
    while (tokens && tokens->type != TOKEN_EOF)
    {
        handle_pipe_errors(&tokens);
        if (!tokens || tokens->type == TOKEN_EOF)
        break;
        tokens = ft_control_token(tokens, all);  
        if (check_parse_errors(all))
        return NULL;
    }
    
    cmd = all->cmd_list;

    return cmd;
}
