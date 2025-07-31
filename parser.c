/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:01:38 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/31 18:02:59 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h" 

// Expansion sonrasında lexer ile tekrar parse etdiyoruz
char **parse_expanded_string_to_argv(char *expanded_str)
{
    if (!expanded_str || ft_strlen(expanded_str) == 0)
        return NULL;
    
    // Kendi lexer'ını kullan
    t_token *tokens = tokenize_input(expanded_str);
    if (!tokens)
        return NULL;
    
    // Token'ları argv array'ine dönüştür
    int argc = 0;
    int capacity = 8;
    char **argv = gb_malloc(sizeof(char*) * capacity);
    if (!argv)
    {
        gc_free(tokens);
        return NULL;
    }
    
    t_token *current = tokens;
    while (current && current->type != TOKEN_EOF)
    {
        if (current->type == TOKEN_WORD)
        {
            if (argc >= capacity - 1)
            {
                capacity *= 2;
                char **new_argv = gb_malloc(sizeof(char*) * capacity);
                if (!new_argv)
                {
                    gc_free(tokens);
                    return NULL;
                }
                int i = 0;
                while(i < argc)
                {
                    new_argv[i] = argv[i];
                    i++;
                }
                argv = new_argv;
            }
            argv[argc++] = ft_strdup(current->value);
        }
        current = current->next;
    }
    argv[argc] = NULL;
    
    gc_free(tokens);
    return argv;
}

// argv array'ini free eden fonksiyon
void free_argv_array(char **argv)
{
    if(argv)
        gc_free(argv);
}

// argv'yi expansion sonrasında yeniden oluşturan fonksiyon
char **rebuild_argv_with_expansion(char **original_argv, char *expanded_str, int position)
{
    char **expanded_argv = parse_expanded_string_to_argv(expanded_str);
    if (!expanded_argv || !expanded_argv[0])
    {
        if (expanded_argv)
            gc_free(expanded_argv);
        return original_argv;
    }
    
    // Orijinal argv'nin uzunluğunu hesapla
    int original_argc = 0;
    while (original_argv[original_argc])
        original_argc++;
    
    // Expanded argv'nin uzunluğunu hesapla
    int expanded_argc = 0;
    while (expanded_argv[expanded_argc])
        expanded_argc++;
    
    // Yeni argv için yer ayır
    int new_argc = original_argc + expanded_argc - 1; // -1 çünkü bir elemanı değiştiriyoruz
    char **new_argv = gb_malloc(sizeof(char*) * (new_argc + 1));
    if (!new_argv)
    {
        gc_free(expanded_argv);
        return original_argv;
    }
    
    int new_idx = 0;
    
    // position'dan önceki elemanları kopyala
    for (int i = 0; i < position; i++)
        new_argv[new_idx++] = ft_strdup(original_argv[i]);
    
    // Expanded elemanları ekle
    for (int i = 0; i < expanded_argc; i++)
        new_argv[new_idx++] = ft_strdup(expanded_argv[i]);
    
    // position'dan sonraki elemanları kopyala
    for (int i = position + 1; i < original_argc; i++)
        new_argv[new_idx++] = ft_strdup(original_argv[i]);
    
    new_argv[new_idx] = NULL;
    gc_free(expanded_argv);
    
    return new_argv;
}

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
    cmd->heredoc_fd = 0;
}

void add_redirection(t_parser *cmd, t_redir_type type, char *filename)
{
    t_redirection *new_redir = gb_malloc(sizeof(t_redirection));
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
    new_redir->next = NULL;

    if (!cmd->redirs)
        cmd->redirs = new_redir;
    else
    {
        t_redirection *temp = cmd->redirs;
        while (temp->next)
            temp = temp->next;
        temp->next = new_redir;
    }
}

void ft_redir_in(t_token *tokens, t_parser *cmd)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
        add_redirection(cmd, REDIR_IN, tokens->value);
    else
        cmd->parse_error = 1;
}

void ft_redir_out(t_token *tokens, t_parser *cmd)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
        add_redirection(cmd, REDIR_OUT, tokens->value);
    else
        cmd->parse_error = 1;
}

void ft_redir_append(t_token *tokens, t_parser *cmd)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
        add_redirection(cmd, REDIR_APPEND, tokens->value);
    else
        cmd->parse_error = 1;
}

void ft_redir_heredoc(t_token *tokens, t_parser *cmd)
{
    tokens = tokens->next;
    if (tokens && tokens->type == TOKEN_WORD)
        add_redirection(cmd, REDIR_HEREDOC, tokens->value);
    else
        cmd->parse_error = 1;
}

void ft_loop_3(t_token **tokens, t_parser *cmd, int *argc)
{
    int i;
    char **new_argv;

    i = 0;
    if ((*tokens)->type == TOKEN_WORD)
    {
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
        cmd->argv[(*argc)++] = ft_strdup((*tokens)->value);
        if (!cmd->argv[(*argc) - 1])
        {
            perror("strdup failed for argv element");
            exit(EXIT_FAILURE);
        }
    }
}

void ft_loop_2(t_token **tokens, t_parser *cmd,int *argc)
{
    if ((*tokens)->type == TOKEN_REDIR_APPEND)
    {
        ft_redir_append(*tokens, cmd);
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
        ft_loop_3(tokens,cmd,argc);

}

void ft_loop(t_token **tokens, t_parser *cmd, int *argc)
{
    if (cmd->parse_error)
        printf("bash: syntax error near unexpected token `<'\n"); 

    if ((*tokens)->type == TOKEN_REDIR_IN)
    {
        ft_redir_in(*tokens, cmd);
        if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
            *tokens = (*tokens)->next;
    }
    else if ((*tokens)->type == TOKEN_REDIR_OUT)
    {
        ft_redir_out(*tokens, cmd);
        if ((*tokens)->next && (*tokens)->next->type == TOKEN_WORD)
            *tokens = (*tokens)->next;
    }
    else
        ft_loop_2(tokens,cmd,argc);

    *tokens = (*tokens)->next;
}


t_token *ft_control_token(t_token *tokens, t_parser **cmd_list, t_parser **last_cmd)
{
    int argc;
    t_parser *cmd;

    argc = 0;
    cmd = malloc(sizeof(t_parser));
    if (!cmd)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    ft_clean_init(cmd);
    while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)
        ft_loop(&tokens, cmd, &argc);
    cmd->argv[argc] = NULL;
    if (!(*cmd_list))
        *cmd_list = cmd;
    else
        (*last_cmd)->next = cmd;
    *last_cmd = cmd;
    if (tokens && tokens->type == TOKEN_PIPE)
        tokens = tokens->next;
    return tokens;
}

t_parser *parse_tokens(t_token *tokens)
{
    t_parser *cmd_list;
    t_parser *last_cmd;

    cmd_list = NULL;
    last_cmd = NULL;
    while (tokens && tokens->type != TOKEN_EOF)
    {
        while (tokens && tokens->type == TOKEN_PIPE)
        {
            printf("bash: syntax error near unexpected token `|'\n");
            tokens = tokens->next;
        }
        if (!tokens || tokens->type == TOKEN_EOF)
            break;

        tokens = ft_control_token(tokens, &cmd_list, &last_cmd);
    }
    return cmd_list;
}
// Lexer tabanlı expansion fonksiyonu
void expand_parser_list(t_parser *cmd_list, t_env *env_list)
{
    t_parser *current = cmd_list;
    
    while (current)
    {
        // Expand argv arguments
        if (current->argv)
        {
            int i = 0;
            while (current->argv[i])
            {
                if (ft_strchr(current->argv[i], '$'))
                {
                    char *expanded = expand_with_quotes(current->argv[i], env_list);
                    if (expanded && ft_strlen(expanded) > 0)
                    {
                        // Lexer ile tekrar parse et ve argv'yi yeniden oluştur
                        char **new_argv = rebuild_argv_with_expansion(current->argv, expanded, i);
                        if (new_argv != current->argv)
                        {
                            // Eski argv'yi free etçez işimiz bitti
                            for (int j = 0; current->argv[j]; j++)
                                gc_free(current->argv[j]);
                            gc_free(current->argv);
                            current->argv = new_argv;
                            
                            // argv_cap'i güncelledik
                            int new_argc = 0;
                            while (new_argv[new_argc])
                                new_argc++;
                            current->argv_cap = new_argc + 1;
                        }
                        gc_free(expanded);
                    }
                    else
                    {
                        if (expanded)
                            gc_free(expanded);
                    }
                }
                i++;
            }
        }
        
        // Expand redirection filenames
        t_redirection *redir = current->redirs;
        while (redir)
        {
            if (ft_strchr(redir->filename, '$'))
            {
                char *expanded = expand_with_quotes(redir->filename, env_list);
                if (expanded)
                {
                    redir->filename = expanded;
                }
            }
            redir = redir->next;
        }
        
        current = current->next;
    }
}

void print_cmds(t_parser *cmd_list)
{
    int a = 1;
    while (cmd_list != NULL)
    {
        printf("Komut %d:\n", a);
        printf("  Komut ve Argümanlar:");
        int i = 0;
        while (cmd_list->argv && cmd_list->argv[i])
        {
            printf(" %s", cmd_list->argv[i]);
            i++;
        }
        if (!cmd_list->argv || !cmd_list->argv[0])
            printf(" (yok)");
        printf("\n");

        t_redirection *redir = cmd_list->redirs;
        while (redir)
        {
            if (redir->type == REDIR_IN)
            {
                printf("  < Redirection: %s\n", redir->filename);
            }
            else if (redir->type == REDIR_OUT)
            {
                printf("  > Redirection: %s (Overwrite)\n", redir->filename);
            }
            else if (redir->type == REDIR_APPEND)
            {
                printf("  >> Redirection: %s (Append)\n", redir->filename);
            }
            else if (redir->type == REDIR_HEREDOC)
            {
                printf("  << Heredoc: %s\n", redir->filename);
            }
            redir = redir->next;
        }

        printf("\n");
        cmd_list = cmd_list->next;
        a++;
    }
}

void print(t_parser *c)
{
    t_parser *current_cmd = c;
    int a = 0;
    while (current_cmd != NULL)
    {
        if (current_cmd->argv && current_cmd->argv[0] != NULL)
        {
            printf("%d ",a);
            int i = 0;
            while (current_cmd->argv[i] != NULL)
            {
                printf("'%s' ", current_cmd->argv[i]);
                i++;
            }
        
            printf("(%s) ",current_cmd->argv[i]);
            printf("\n");
        }
        else
        {
            printf("Bu komutun argümanı yok.\n");
        }
        current_cmd = current_cmd->next;
        a++;
    }
}