/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 00:23:50 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/11 05:04:14 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// abi burda değişken karakterini kontrol ediyoruz
int is_valid_char(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
}

// stringi expand ettikten sonra boşluklara göre böl

char **split_expanded_string(char *str)
{
    char **result;
    int count;
    int i;
    int start;
    int len;

    if (!str)
        return (NULL);
    
    len = ft_strlen(str);
    result = gb_malloc(sizeof(char*) * (len / 2 + 10));
    if (!result)
        return (NULL);
    
    count = 0;
    i = 0;
    
    while (str[i])
    {
        while (str[i] == ' ' || str[i] == '\t')
            i++;
        if (!str[i])
            break;
            
        start = i;
        while (str[i] && str[i] != ' ' && str[i] != '\t')
            i++;
            
        result[count] = gb_malloc(i - start + 1);
        if (!result[count])
            return (NULL);
        ft_memcpy(result[count], str + start, i - start);
        result[count][i - start] = '\0';
        count++;
    }
    result[count] = NULL;
    return (result);
}

char *expand_variables(char *str, t_env *env_list)
{
    char *res;
    char *expanded;
    char *new_res;
    int res_len;
    int res_cap;
    int i;
    int add_len;
    int str_len;

    if (!str)
        return NULL;
    
    str_len = ft_strlen(str);
    res_cap = str_len + 1;
    
    res = gb_malloc(res_cap);
    if (!res)
        return NULL;
    
    res[0] = '\0';
    res_len = 0;
    i = 0;
    
    while (str[i])
    {
        if (str[i] == '$')
        {
            expanded = handle_dollar(str, &i, env_list);
            if (expanded)
            {
                add_len = ft_strlen(expanded);
                if (res_len + add_len >= res_cap)
                {
                    res_cap = res_len + add_len + 1;
                    new_res = gb_malloc(res_cap);
                    if (!new_res)
                    {
                        gc_free(res);
                        gc_free(expanded);
                        return NULL;
                    }
                    ft_memcpy(new_res, res, res_len);
                    gc_free(res);
                    res = new_res;
                }
                ft_memcpy(res + res_len, expanded, add_len);
                res_len += add_len;
                res[res_len] = '\0';
                gc_free(expanded);
            }
        }
        else
        {
            if (res_len + 1 >= res_cap)
            {
                res_cap = res_len + 2;
                new_res = gb_malloc(res_cap);
                if (!new_res)
                {
                    gc_free(res);
                    return NULL;
                }
                ft_memcpy(new_res, res, res_len);
                gc_free(res);
                res = new_res;
            }
            res[res_len] = str[i];
            res_len++;
            res[res_len] = '\0';
            i++;
        }
    }
    return res;
}

char *handle_dollar(char *str, int *i, t_env *env_list)
{
    char *var_name;
    char *expanded;
    int start_i;
    int len;
    t_env *env_node;
    char *export_value;

    (*i)++;
    if (!str[*i])
        return ft_strdup("$");
    
    if (str[*i] == '?')
    {
        (*i)++;
        return ft_itoa(get_last_exit_status()); // Getter kullan
    }
    
    if (str[*i] == '$')
    {
        (*i)++;
        return ft_itoa(getpid());
    }
    
    if (isalpha(str[*i]) || str[*i] == '_')
    {
        start_i = *i;
        while (str[*i] && (isalnum(str[*i]) || str[*i] == '_'))
            (*i)++;
        
        len = *i - start_i;
        if (len <= 0)
            return ft_strdup("");
            
        var_name = gb_malloc(len + 1);
        if (!var_name)
            return ft_strdup("");
            
        ft_memcpy(var_name, str + start_i, len);
        var_name[len] = '\0';
        
        // Önce env'den ara
        env_node = find_env(env_list, var_name);
        if (env_node && env_node->value)
        {
            expanded = ft_strdup(env_node->value);
            gc_free(var_name);
            return expanded ? expanded : ft_strdup("");
        }
        
        // Sonra export'tan ara
        export_value = find_export_value(var_name);
        if (export_value)
        {
            expanded = ft_strdup(export_value);
            gc_free(var_name);
            return expanded ? expanded : ft_strdup("");
        }
        
        // Bulunamadı, boş string döndür
        gc_free(var_name);
        return ft_strdup("");
    }
    
    // Geçersiz karakter sonrası $, sadece $ döndür
    return ft_strdup("$");
}

char *handle_simple(const char *str, int *i, t_env *env_list)
{
    int len;
    char *var_name;
    t_env *env_var;
    char *result;

    (*i)++;
    len = 0;
    while (is_valid_char(str[*i + len]))
        len++;
    
    if (len == 0)
    {
        (*i)++;
        return (ft_strdup(""));
    }
    
    var_name = gb_malloc(len + 1);
    if (!var_name)
        return (NULL);
    ft_memcpy(var_name, str + *i, len);
    var_name[len] = '\0';
    *i += len;
    
    env_var = find_env(env_list, var_name);
    
    if (env_var == NULL)
    {
        result = ft_strdup("");
        gc_free(var_name);
        return (result);
    }
    
    gc_free(var_name);
    
    if (env_var->value == NULL)
        return (ft_strdup(""));
    
    return (ft_strdup(env_var->value));
}

char *push_char_res(char *res, char c, int *res_len, int *res_cap)
{
    char *new_res;

    if (*res_len + 1 >= *res_cap)
    {
        *res_cap = *res_len + 2;
        new_res = gb_malloc(*res_cap);
        if (!new_res)
            return (res);
        if (res)
        {
            ft_memcpy(new_res, res, *res_len);
            gc_free(res);
        }
        else
            new_res[0] = '\0';
        res = new_res;
    }
    res[*res_len] = c;
    (*res_len)++;
    res[*res_len] = '\0';
    return (res);
}

static char *append_string_res(char *result, char *str, int *res_len, int *res_cap)
{
    int add_len;
    char *new_res;

    add_len = ft_strlen(str);
    if (*res_len + add_len >= *res_cap)
    {
        *res_cap = *res_len + add_len + 1;
        new_res = gb_malloc(*res_cap);
        if (!new_res)
            return (result);
        
        if (result)
        {
            ft_memcpy(new_res, result, *res_len);
            gc_free(result);
        }
        else
            new_res[0] = '\0';
        
        result = new_res;
    }
    
    ft_memcpy(result + *res_len, str, add_len);
    *res_len += add_len;
    result[*res_len] = '\0';
    return (result);
}

char *expand_with_quotes(char *str, t_env *env_list)
{
    int str_len;
    int res_len;
    int res_cap;
    int i;
    char quote_char;
    char *result;
    char *expanded;

    if (!str)
        return NULL;
    
    str_len = ft_strlen(str);
    res_cap = str_len + 1;
    
    result = gb_malloc(res_cap);
    if (!result)
        return NULL;
    
    result[0] = '\0';
    res_len = 0;
    i = 0;
    quote_char = 0;
    
    while (str[i])
    {
        if (str[i] == '\'' || str[i] == '"')
        {
            if (quote_char == 0)
            {
                quote_char = str[i];
                i++;
            }
            else if (quote_char == str[i])
            {
                quote_char = 0;
                i++;
            }
            else
            {
                result = push_char_res(result, str[i], &res_len, &res_cap);
                i++;
            }
        }
        else if (str[i] == '$' && quote_char != '\'')
        {
            expanded = handle_dollar(str, &i, env_list);
            if (expanded)
            {
                result = append_string_res(result, expanded, &res_len, &res_cap);
                gc_free(expanded);
            }
        }
        else
        {
            result = push_char_res(result, str[i], &res_len, &res_cap);
            i++;
        }
    }
    
    return result;
}

char *expand_heredoc_line(char *str, t_env *env_list)
{
    int len;
    int res_len;
    int res_cap;
    int i;
    char quote_char;
    char *result;
    char *expanded;

    if (!str)
        return NULL;
    
    len = ft_strlen(str);
    res_cap = len + 1;
    
    result = gb_malloc(res_cap);
    if (!result)
        return NULL;
    
    result[0] = '\0';
    res_len = 0;
    i = 0;
    quote_char = 0;
    
    while (str[i])
    {
        if (str[i] == '\'' || str[i] == '\"')
        {
            result = push_char_res(result, str[i], &res_len, &res_cap);
            if (quote_char == 0)
                quote_char = str[i];
            else if (quote_char == str[i])
                quote_char = 0;
            i++;
        }
        else if (str[i] == '$')
        {
            expanded = handle_dollar(str, &i, env_list);
            if (expanded)
            {
                result = append_string_res(result, expanded, &res_len, &res_cap);
                gc_free(expanded);
            }
        }
        else
        {
            result = push_char_res(result, str[i], &res_len, &res_cap);
            i++;
        }
    }
    return result;
}