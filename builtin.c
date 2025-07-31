/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 05:36:30 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/31 17:20:04 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char *remove_quotes(char *str)
{
    if (!str)
        return NULL;
    
    int len = strlen(str);
    if (len == 0)
        return str;
    
    // Önce quote'ların doğru eşleşip eşleşmediğini kontrol et
    int i = 0;
    char in_quote = 0;
    int quote_count_single = 0;
    int quote_count_double = 0;
    
    // Quote sayısını ve eşleşmesini kontrol et
    while (i < len)
    {
        if (str[i] == '"' && in_quote != '\'')
        {
            if (in_quote == '"')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '"';
            quote_count_double++;
        }
        else if (str[i] == '\'' && in_quote != '"')
        {
            if (in_quote == '\'')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '\'';
            quote_count_single++;
        }
        i++;
    }
    
    // Eğer quote açık kalmışsa veya tek sayıda quote varsa hata
    if (in_quote != 0 || (quote_count_single % 2 != 0) || (quote_count_double % 2 != 0))
    {
        printf("bash: syntax error: unclosed quotes\n");
        return NULL;
    }
    
    // Quote'ları kaldır
    char *result = malloc(len + 1);
    if (!result)
        return str;
    
    i = 0;
    int j = 0;
    in_quote = 0;
    
    while (i < len)
    {
        char current = str[i];
        
        if (current == '"' || current == '\'')
        {
            if (in_quote == 0)
            {
                in_quote = current;
            }
            else if (in_quote == current)
            {
                in_quote = 0;
            }
            else
            {
                result[j++] = current;
            }
        }
        else
        {
            result[j++] = current;
        }
        i++;
    }
    
    result[j] = '\0';
    
    if (j == len)
    {
        free(result);
        return str;
    }
    
    return result;
}

int built_echo(t_parser *cmd)
{
    int i = 1;
    int newline = 1;
    int first_arg = 1;
    
    // -n flag kontrolü
    if(cmd->argv[i] && ft_strcmp(cmd->argv[i], "-n") == 0)
    {
        newline = 0;
        i++; 
    }
    
    while(cmd->argv[i])
    {
        char *processed_arg = remove_quotes(cmd->argv[i]);
        
        // Eğer quote hatası varsa (NULL döndürülmüşse) fonksiyondan çık
        if (!processed_arg)
            return 1; // Error exit code
        // Aralarına boşluk ekle (ilk argüman değilse)
        if (!first_arg)
            printf(" ");
        
        printf("%s", processed_arg);
        
        // Eğer yeni string oluşturulduysa free et
        if (processed_arg != cmd->argv[i])
            free(processed_arg);
        
        first_arg = 0;
        i++;
    }
    
    if(newline)
        printf("\n");
    
    return 0;
}

int built_cd(t_parser *cmd, t_env **env_list)
{
    char *oldpwd;
    char *pwd;
    char cwd[1024];
    
    // Get current directory before changing
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    
    oldpwd = ft_strdup(cwd);
    
    // If no argument, cd to HOME
    if (cmd->argv[1] == NULL) 
    {
        pwd = get_env_value(*env_list, "HOME");
        if (!pwd)
        {
            fprintf(stderr, "cd: HOME not set\n");
            free(oldpwd);
            return 1;
        }
    }
    else
    {
        // Quote'ları kaldır
        pwd = remove_quotes(cmd->argv[1]);
        if (!pwd) // Quote hatası
        {
            free(oldpwd);
            return 1;
        }
    }
    
    if (chdir(pwd) != 0)
    {
        perror("cd");
        if (pwd != cmd->argv[1])
            free(pwd);
        free(oldpwd);
        return 1;
    }
    
    // Update PWD and OLDPWD environment variables
    set_env_value(env_list, "OLDPWD", oldpwd);
    
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        set_env_value(env_list, "PWD", cwd);
    
    if (pwd != cmd->argv[1])
        free(pwd);
    free(oldpwd);
    return 0;
}

int builtin_pwd(void)
{
    char cwd[1024];
    if(getcwd(cwd,sizeof(cwd)) != NULL)
    {
        printf("%s\n",cwd);
        return 0;
    }
    else
    {
        perror("pwd");
        return 1;
    }
}

int builtin_exit(t_parser *cmd)
{
    int code = 0;
    if(cmd->argv[1] != NULL)
    {
        char *processed_arg = remove_quotes(cmd->argv[1]);
        code = atoi(processed_arg);
        if (processed_arg != cmd->argv[1])
            free(processed_arg);
    }
    printf("exit\n");
    exit(code);
    return 0;
}

int builtin_export(t_parser *cmd, t_env **env_list)
{
    if (!cmd->argv[1]) // No arguments, print all exported variables
    {
        t_env *current = *env_list;
        while (current)
        {
            printf("declare -x %s=\"%s\"\n", current->key, current->value ? current->value : "");
            current = current->next;
        }
        return 0;
    }

    int i = 1;
    while (cmd->argv[i])
    {
        char *arg = cmd->argv[i];
        char *eq_pos = ft_strchr(arg, '=');
        int valid = 1;
        int j = 0;

        // POSIX compliant validation - validate the key part before =
        if (!arg[0] || (!((arg[0] >= 'a' && arg[0] <= 'z') || 
                          (arg[0] >= 'A' && arg[0] <= 'Z') || 
                          arg[0] == '_')))
        {
            valid = 0;
        }
        else
        {
            // Check characters before '=' or entire string if no '='
            int check_len;
            if(eq_pos)
                check_len = eq_pos - arg;
            else
                check_len = ft_strlen(arg);
            while (j < check_len)
            {
                char c = arg[j];
                if (!((c >= 'a' && c <= 'z') || 
                      (c >= 'A' && c <= 'Z') || 
                      (c >= '0' && c <= '9') || 
                      c == '_'))
                {
                    valid = 0;
                    break;
                }
                j++;
            }
        }

        if (!valid)
        {
            printf("export: `%s': not a valid identifier\n", arg);
            i++;
            continue;
        }

        if (eq_pos) // Variable assignment: VAR=value
        {
            char *key = strndup(arg, eq_pos - arg);
            char *value = eq_pos + 1;  // Quote'lı değeri al
            
            // Quote'ları kaldır - but keep the original if remove_quotes fails
            char *clean_value = remove_quotes(value);
            if (clean_value)
            {
                set_env_value(env_list, key, clean_value);
                if (clean_value != value)
                    free(clean_value); // Free only if a new string was allocated
            }
            else
            {
                // remove_quotes failed, use original value
                set_env_value(env_list, key, value);
            }
            free(key);
        }
        else // Just export existing variable or create empty one
        {
            // Quote'ları kaldır
            char *clean_arg = remove_quotes(arg);
            char *var_name = clean_arg ? clean_arg : arg;
            
            t_env *existing = find_env(*env_list, var_name);
            if (!existing)
            {
                // Create new variable with empty value
                set_env_value(env_list, var_name, "");
            }
            
            if (clean_arg != arg && clean_arg)
                free(clean_arg);
        }
        
        i++;
    }
    return 0;
}

int builtin_unset(t_parser *cmd, t_env **env_list)
{
    if (!cmd->argv[1])
        return 0;
    
    int i = 1;
    while (cmd->argv[i])
    {
        char *var_name = remove_quotes(cmd->argv[i]);
        int valid = 1;
        int j = 0;
        
        // POSIX: identifier must start with letter or '_', rest can be letter/digit/_
        if (!var_name[0] || (!((var_name[0] >= 'a' && var_name[0] <= 'z') || (var_name[0] >= 'A' && var_name[0] <= 'Z') || var_name[0] == '_')))
            valid = 0;
        else {
            while (var_name[j]) {
                char c = var_name[j];
                if (!( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
                    valid = 0;
                    break;
                }
                j++;
            }
        }
        if (!valid) {
            printf("unset: `%s': not a valid identifier\n", var_name);
            if (var_name != cmd->argv[i])
                free(var_name);
            i++;
            continue;
        }
        unset_env_value(env_list, var_name);
        
        if (var_name != cmd->argv[i])
            free(var_name);
        i++;
    }
    return 0;
}

int builtin_env(t_parser *cmd, t_env *env_list)
{
    (void)cmd;
    t_env *current = env_list;
    while (current)
    {
        printf("%s=%s\n", current->key, current->value ? current->value : "");
        current = current->next;
    }
    return 0;
}