#include "shell.h"

// echo komutu - -n flag'i de destekliyor
void built_echo(t_parser *cmd)
{
    int i;
    int newline;
    
    i = 1;
    newline = 1;
   
    // Tüm -n flaglerini kontrol et (bash davranışı)
    while (cmd->argv[i] && cmd->argv[i][0] == '-' && cmd->argv[i][1] == 'n')
    {
        int j = 2;
        int valid_flag = 1;
        
        while (cmd->argv[i][j])
        {
            if (cmd->argv[i][j] != 'n')
            {
                valid_flag = 0;
                break;
            }
            j++;
        }
        
        if (valid_flag)
        {
            newline = 0; 
            i++; 
        }
        else
            break; 
    }
    
    while(cmd->argv[i])
    {
        write(STDOUT_FILENO, cmd->argv[i], ft_strlen(cmd->argv[i]));
        if(cmd->argv[i + 1] != NULL)
            write(STDOUT_FILENO, " ", 1);
        i++;
    }

    if (newline)
        write(STDOUT_FILENO, "\n", 1);
    
    set_last_exit_status(0);
}

int built_cd(t_parser *cmd)
{
    char *home_dir;
    char *current_pwd;
    char *old_pwd;
    int arg_count = 0;
    int exit_code = 0;
    
    // Current PWD'yi al
    current_pwd = getcwd(NULL, 0);
    old_pwd = current_pwd ? ft_strdup(current_pwd) : NULL;
    
    // boş arg de patlıyordu fix
    while (cmd->argv[arg_count])
        arg_count++;
    
    // cd + 2 path geçersiz 
    if (arg_count > 2)
    {
        printf("bash: cd: too many arguments\n");
        exit_code = 1;
    }
    else if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
    {
        // sadece cd yazıldığında veya cd ile boş argüman verildiğinde home'a git
        home_dir = getenv("HOME");
        if (home_dir == NULL)
        {
            printf("cd: HOME not set\n");
            exit_code = 1;
        }
        else if (chdir(home_dir) != 0)
        {
            perror("cd");
            exit_code = 1;
        }
    }
    else
    {
        if (chdir(cmd->argv[1]) != 0)
        {
            perror("cd");
            exit_code = 1;
        }
    }
    
    // PWD güncelleme - sadece chdir başarılıysa
    if (exit_code == 0)
    {
        char *new_pwd = getcwd(NULL, 0);
        if (new_pwd)
        {
            setenv("PWD", new_pwd, 1);
            if (old_pwd)
                setenv("OLDPWD", old_pwd, 1);
            free(new_pwd);
        }
    }
    
    // Cleanup
    if (current_pwd) 
        free(current_pwd);
    if (old_pwd) 
        gc_free(old_pwd);
    
    set_last_exit_status(exit_code);
    return exit_code;
}

void builtin_pwd(void)
{
    char *cwd = getcwd(NULL, 0);
    
    if (cwd)
    {
        size_t len = strlen(cwd);
        ssize_t write_result = write(STDOUT_FILENO, cwd, len);
        
        if (write_result == -1)
        {
            perror("write");
            set_last_exit_status(1);
        }
        else
        {
            write_result = write(STDOUT_FILENO, "\n", 1);
            if (write_result == -1)
            {
                perror("write");
                set_last_exit_status(1);
            }
            else
            {
                set_last_exit_status(0);
            }
        }
        free(cwd);
    }
    else
    {
        perror("pwd");
        set_last_exit_status(1);
    }
}

// is_numeric_string fonksiyonu - builtin_exit'ten önce tanımla
int is_numeric_string(char *str)
{
    int i = 0;
    
    if (!str || str[0] == '\0')
        return 0;
    
    // Sign kontrolü
    if (str[0] == '+' || str[0] == '-')
        i = 1;
    
    // Sadece sign varsa invalid
    if (str[i] == '\0')
        return 0;
    
    // Tüm karakterler digit olmalı
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return 0;
        i++;
    }
    
    return 1;
}

// builtin_exit fonksiyonu - TEK TANIMLAMA
void builtin_exit(t_parser *cmd)
{
    int exit_code = 0;
    
    // exit yazdır - bash davranışı
    printf("exit\n");
    
    if (cmd->argv[1] && cmd->argv[2])
    {
        // Çok fazla argüman hatası
        printf("bash: exit: too many arguments\n");
        set_last_exit_status(1);
        return; // Shell'i kapatma, sadece hata ver
    }
    
    if (cmd->argv[1])
    {
        if (!is_numeric_string(cmd->argv[1]))
        {
            // Numeric olmayan argüman
            printf("bash: exit: %s: numeric argument required\n", cmd->argv[1]);
            exit_code = 2; // Bash'de 2 döner
        }
        else
        {
            exit_code = ft_atoi(cmd->argv[1]);
            // Bash'de exit code 0-255 arasında mod alınır
            exit_code = ((exit_code % 256) + 256) % 256;
        }
    }
    else
    {
        // Argüman yoksa, son exit status'u kullan
        exit_code = get_last_exit_status();
    }
    
    // FD'leri temizle
    if (cmd->fd_i != -1)
        close(cmd->fd_i);
    if (cmd->fd_o != -1)
        close(cmd->fd_o);
    close_all_fds_except_std(cmd);
    
    // Garbage collector cleanup
    gb_free_all();
    env_gb_free_all();
    
    // Readline cleanup
    rl_clear_history();
    
    exit(exit_code);
}

static void env_bubble_sort(char **env_array, int count)
{
    int i, j;
    char *temp;
    int swapped;
    
    if (!env_array || count <= 1)
        return;
    
    for (i = 0; i < count - 1; i++)
    {
        swapped = 0;
        for (j = 0; j < count - i - 1; j++)
        {
            if (ft_strcmp(env_array[j], env_array[j + 1]) > 0)
            {
                temp = env_array[j];
                env_array[j] = env_array[j + 1];
                env_array[j + 1] = temp;
                swapped = 1;
            }
        }
        if (swapped == 0)
            break;
    }
}

static char **create_sorted_env_array(t_env *env_list)
{
    int count = 0;
    t_env *current = env_list;
    char **env_array;
    int i = 0;
    
    while (current)
    {
        if (current->value) // Sadece değeri olan env'ları say
            count++;
        current = current->next;
    }
    
    if (count == 0)
        return NULL;

    env_array = gb_malloc(sizeof(char*) * (count + 1));
    if (!env_array)
        return NULL;

    current = env_list;
    while (current && i < count)
    {
        if (current->value)
        {
            int len = ft_strlen(current->key) + ft_strlen(current->value) + 2; // +2 for '=' and '\0'
            env_array[i] = gb_malloc(len);
            if (!env_array[i])
                return NULL;
            ft_strcpy(env_array[i], current->key);
            ft_strcat(env_array[i], "=");
            ft_strcat(env_array[i], current->value);
            i++;
        }
        current = current->next;
    }
    env_array[i] = NULL;
    
    env_bubble_sort(env_array, count);
    
    return env_array;
}

void builtin_env(t_env *env_list)
{
    char **sorted_env = create_sorted_env_array(env_list);
    int i = 0;
    
    if (!sorted_env)
    {
        set_last_exit_status(0);
        return;
    }
    
    while (sorted_env[i])
    {
        printf("%s\n", sorted_env[i]);
        i++;
    }

    i = 0;
    while (sorted_env[i])
    {
        gc_free(sorted_env[i]);
        i++;
    }
    gc_free(sorted_env);
    set_last_exit_status(0);
}