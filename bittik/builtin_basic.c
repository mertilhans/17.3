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
        printf("%s", cmd->argv[i]);
        if(cmd->argv[i + 1] != NULL)
            printf(" ");
        i++;
    }

    
    if (newline)
        printf("\n");
}

int built_cd(t_parser *cmd)
{
    char *home_dir;
    char *current_pwd;
    char *old_pwd;
    int arg_count = 0;
    
    // Current PWD'yi al
    current_pwd = getcwd(NULL, 0);//getcwd null döndürmeyi kontrol et
    old_pwd = current_pwd ? ft_strdup(current_pwd) : NULL;
    
    // boş arg de patlıyordu fix
    while (cmd->argv[arg_count])
        arg_count++;
    
    // cd + 2 path         geçersiz 
    if (arg_count > 2)
    {
        printf("bash: cd: too many arguments\n");
        if (current_pwd) free(current_pwd);
        if (old_pwd) gc_free(old_pwd);
        return 1;
    }
    
    if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
    {
        // sadece cd yazıldığında veya cd ile boş argüman verildiğinde home'a git
        home_dir = getenv("HOME");
        if (home_dir == NULL)
        {
            printf("cd: HOME not set\n");
            if (current_pwd) free(current_pwd);
            if (old_pwd) gc_free(old_pwd);
            return 1;
        }
        if (chdir(home_dir) != 0)
        {
            perror("cd");
            if (current_pwd) free(current_pwd);
            if (old_pwd) gc_free(old_pwd);
            return 1;
        }
    }
    else
    {
        if (chdir(cmd->argv[1]) != 0)
        {
            perror("cd");
            if (current_pwd) free(current_pwd);
            if (old_pwd) gc_free(old_pwd);
            return 1;
        }
    }
    
    // PWD ve OLDPWD güncelle
    char *new_pwd = getcwd(NULL, 0);
    if (new_pwd)
    {
        // Bu fonksiyonları execute.c'den çağıramayız, global export list kullanmalıyız
        setenv("PWD", new_pwd, 1);
        if (old_pwd)
            setenv("OLDPWD", old_pwd, 1);
        free(new_pwd);
    }
    
    if (current_pwd) free(current_pwd);
    if (old_pwd) gc_free(old_pwd);
    return 0;
}
#include <errno.h>
void builtin_pwd(void)
{
    char *cwd = getcwd(NULL, 0);
    
    if (cwd)
    {
        // printf yerine write kullan
        size_t len = strlen(cwd);
        if (write(STDOUT_FILENO, cwd, len) == -1)
        {
            //EPIPE hatası sessizce yoksayılır
            if (errno != EPIPE)
                perror("write");
        }
        else
        {
            // Newline ekle
            if (write(STDOUT_FILENO, "\n", 1) == -1)
            {
                if (errno != EPIPE)
                    perror("write");
            }
        }
        free(cwd);
    }
    else
        perror("pwd");
}

void builtin_exit(t_parser *cmd)
{
    int exit_code;
    int i;
    exit_code = 0;
    
    if (cmd->argv[1] && cmd->argv[2])
    {
        printf("bash: exit: too many arguments\n");
        return(exit(2));
    }
    
    if (cmd->argv[1])
    {
        //sadece sayı mı diye bakçez
        i = 0;
        if (cmd->argv[1][0] == '-' || cmd->argv[1][0] == '+')
            i = 1;
        
        while (cmd->argv[1][i])
        {
            if (cmd->argv[1][i] < '0' || cmd->argv[1][i] > '9')
            {
                printf("bash: exit: %s: numeric argument required\n", cmd->argv[1]);
                  close(cmd->fd_i);
            close(cmd->fd_o);
            
            // Builtin sonrası tüm FD'leri temizle
            //tt(data);
            close_all_fds_except_std(cmd);
            gb_free_all();
            env_gb_free_all();
                exit(2);
            }
            i++;
        }
        
        // Geçerli sayı ise convert et
        exit_code = ft_atoi(cmd->argv[1]);
        // Exit kodu 0-255 arasında olmalı (modulo 256)
        // Negatif sayılar için doğru modulo işlemi
        exit_code = ((exit_code % 256) + 256) % 256; // syntax bu şekilde olmasının sebebi işlem önceliği
    }
    close(cmd->fd_i);
    close(cmd->fd_o);
    
    // Builtin sonrası tüm FD'leri temizle
    //tt(data);
    close_all_fds_except_std(cmd);
    gb_free_all();
    env_gb_free_all();
  
    exit(exit_code);
}

void builtin_env(t_env *env_list)
{
    t_env *current = env_list;
    
    while (current)
    {
        if (current->value)
            printf("%s=%s\n", current->key, current->value);
        current = current->next;
    }
}
