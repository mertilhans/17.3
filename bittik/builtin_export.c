#include "shell.h"

// export listesinin hepsini yazdır abi
void export_print_all(t_export **export_list)
{
    t_export *current = *export_list;
    while (current)
    {
        printf("declare -x %s", current->key);
        if (current->value != NULL)
            printf("=\"%s\"", current->value);
        printf("\n");
        current = current->next;
    }
}

// değer birleştirme işlemi  export TEST="hello world" gibiler için
char *export_build_value(t_parser *cmd, int *i, char *value)
{
    char *full_value;
    int j;
    int total_len;
    
   
    total_len = ft_strlen(value);
    j = *i + 1;
    while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
    {
        total_len += ft_strlen(cmd->argv[j]) + 1; 
        j++;
    }
    
    full_value = gb_malloc(total_len + 1); 
    ft_strcpy(full_value, value);
    j = *i + 1;
    while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
    {
        ft_strcat(full_value, " ");
        ft_strcat(full_value, cmd->argv[j]);
        j++;
    }
    
    *i = j - 1;
    return full_value;
}

// key=value formatındaki argumentları işle reis
void export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
{
    t_export **export_list;
    char *current_arg;
    char *eq_pos;
    char *key;
    char *value;
    
    export_list = get_export_list();
    current_arg = cmd->argv[*i];
    eq_pos = ft_strchr(current_arg, '=');
    
    *eq_pos = '\0';
    key = current_arg;
    value = eq_pos + 1;
    
    if (ft_strlen(value) > 0)
    {
        if (cmd->argv[*i + 1])
        {
            char *full_value = export_build_value(cmd, i, value);
            set_export_value(export_list, key, full_value);
            set_env_value(env_list, key, full_value);
            free(full_value);
        }
        else
        {
            set_export_value(export_list, key, value);
            set_env_value(env_list, key, value);
        }
    }
    else
    {
        // Boş değer atanmış (export var=), boş string olarak ayarla
        set_export_value(export_list, key, "");
        set_env_value(env_list, key, "");
    }
    
    *eq_pos = '=';
}

// ana export fonksiyonu - bu arkadaş çok iş yapıyor
void builtin_export(t_parser *cmd, t_env **env_list)
{
    int i;
    t_export **export_list;
    
    i = 1;
    export_list = get_export_list();
    
    if (!cmd->argv[1])
    {
        export_print_all(export_list);
        return;
    }
    
    while (cmd->argv[i])
    {
        char *current_arg;
        char *eq_pos;
        char *identifier;
        
        current_arg = cmd->argv[i];
        eq_pos = ft_strchr(current_arg, '=');
        identifier = current_arg;
        
        if (eq_pos)
        {
            *eq_pos = '\0'; 
            identifier = current_arg;
        }
        
        if (identifier[0] == '$')
        {
            printf("bash: export: `%s': not a valid identifier\n", identifier);
            if (eq_pos) *eq_pos = '=';
        }
        else if (!((identifier[0] >= 'a' && identifier[0] <= 'z') || 
              (identifier[0] >= 'A' && identifier[0] <= 'Z') || 
              identifier[0] == '_'))
        {
            printf("bash: export: `%s': not a valid identifier\n", identifier);
            if (eq_pos) *eq_pos = '='; 
        }
        else
        {
            if (eq_pos) *eq_pos = '=';
            
            if (eq_pos)
                export_process_keyvalue(cmd, &i, env_list);
            else
                set_export_value(export_list, cmd->argv[i], NULL);
        }
        i++;
    }
}

// değişkeni hem env hem export listesinden kaldır abi
void builtin_unset(t_parser *cmd, t_env **env_list)
{
    int i;
    t_export **export_list;
    
    i = 1;
    export_list = get_export_list();
    
    if (!cmd->argv[1])
    {
        printf("unset: not enough arguments\n");
        return;
    }
    
    while (cmd->argv[i])
    {
        unset_env_value(env_list, cmd->argv[i]);
        unset_export_value(export_list, cmd->argv[i]);
        i++;
    }
}
