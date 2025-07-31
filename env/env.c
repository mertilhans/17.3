/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 05:37:42 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/31 14:56:52 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "../shell.h"

t_env *find_env(t_env *env_list, const char *key) 
{
    t_env *current;
    
    if (!env_list || !key)
    {
        printf("DEBUG find_env: env_list=%p, key=%p\n", (void*)env_list, (void*)key);
        return NULL;
    }
    
    printf("DEBUG find_env: Looking for key='%s'\n", key);
    
    current = env_list;
    int count = 0;
    while (current)
    {
        printf("DEBUG find_env: [%d] comparing '%s' with '%s'\n", count++, current->key, key);
        if (ft_strcmp(current->key, key) == 0)
        {
            printf("DEBUG find_env: FOUND! key='%s' value='%s'\n", current->key, current->value ? current->value : "(null)");
            return current;  // t_env* döndürülüyor
        }
        current = current->next;
    }
    printf("DEBUG find_env: NOT FOUND after checking %d variables\n", count);
    return NULL;
}

t_env   *init_env(char **env)
{
        t_env *env_list = NULL;
        t_env *new_node;
        char *key;
        char *value;
        char *eq_pos;
        int i;
        i = 0;
        while(env[i])
        {
                eq_pos = strchr(env[i], '=');
                if(eq_pos)
                {
                        key = strndup(env[i],eq_pos - env[i]);
                        value = strdup(eq_pos + 1);
                        new_node = malloc(sizeof(t_env));
                        new_node->key = key;
                        new_node->value = value;
                        new_node->next = env_list;
                        env_list = new_node;
                }
                i++;
        }
        return(env_list);
}

char    *get_env_value(t_env *env_list,char *key)
{
    t_env *env_variable = find_env(env_list, key);
    if (env_variable)
        return env_variable->value;
    return NULL;
}

void set_env_value(t_env **env_list, char *key, char *value)
{
    printf("DEBUG set_env_value: Setting key='%s' value='%s'\n", key, value);
    
    t_env *existing = find_env(*env_list, key);
    
    if (existing)
    {
        printf("DEBUG set_env_value: Variable exists, updating value\n");
        free(existing->value);
        existing->value = strdup(value);
    }
    else
    {
        printf("DEBUG set_env_value: Creating new variable\n");
        t_env *new_node = malloc(sizeof(t_env));
        if (!new_node)
        {
            perror("malloc failed");
            return;
        }
        new_node->key = strdup(key);
        new_node->value = strdup(value);
        new_node->next = *env_list;
        *env_list = new_node;
        printf("DEBUG set_env_value: New variable created: key='%s' value='%s'\n", new_node->key, new_node->value);
    }
}

void unset_env_value(t_env **env_list, char *key)
{
    if (!env_list || !*env_list || !key)
        return;
        
    t_env *current = *env_list;
    t_env *prev = NULL;
    
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            if (prev)
                prev->next = current->next;
            else
                *env_list = current->next;
            
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}