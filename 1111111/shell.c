/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:25 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/11 15:49:12 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_tokenizer	*tokenizer_init(char *input)
{
	t_tokenizer	*tokenizer;

	if (!input)
		return (NULL);
	tokenizer = gb_malloc(sizeof(t_tokenizer));
	if (!tokenizer)
		return (NULL);
	tokenizer->input = ft_strdup(input);
	if (!tokenizer->input)
	{
		gc_free(tokenizer);
		return (NULL);
	}
	tokenizer->pos = 0;
	tokenizer->len = ft_strlen(input);
	if (tokenizer->len > 0)
		tokenizer->current = input[0];
	else
		tokenizer->current = '\0';
	return (tokenizer);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = gb_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = ft_strdup(value);
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

static void	init_shlvl(t_env **env_list, t_export **export_list)
{
	t_env	*shlvl_node;
	int		shlvl_value;
	char	*shlvl_str;

	shlvl_node = find_env(*env_list, "SHLVL");
	shlvl_value = 1;
	if (shlvl_node && shlvl_node->value)
		shlvl_value = ft_atoi(shlvl_node->value) + 1;
	shlvl_str = ft_itoa(shlvl_value);
	set_env_value(env_list, "SHLVL", shlvl_str);
	set_export_value(export_list, "SHLVL", shlvl_str);
}

static void	init_pwd(t_env **env_list, t_export **export_list)
{
	t_env	*pwd_node;
	char	*current_pwd;

	pwd_node = find_env(*env_list, "PWD");
	if (!pwd_node)
	{
		current_pwd = getcwd(NULL, 0);
		if (current_pwd)
		{
			set_env_value(env_list, "PWD", current_pwd);
			set_export_value(export_list, "PWD", current_pwd);
			free(current_pwd);
		}
	}
}

static void	init_oldpwd(t_env **env_list, t_export **export_list)
{
	t_env	*oldpwd_node;

	oldpwd_node = find_env(*env_list, "OLDPWD");
	if (!oldpwd_node)
		set_export_value(export_list, "OLDPWD", NULL);
}

void	init_shell_variables(t_env **env_list)
{
	t_export	**export_list;

	export_list = get_export_list();
	init_shlvl(env_list, export_list);
	init_pwd(env_list, export_list);
	init_oldpwd(env_list, export_list);
}

t_env	*initialize_shell(char **env)
{
	t_env		*env_list;
	t_export	**export_list;

	env_list = init_env(env);
	export_list = get_export_list();
	*export_list = init_export_from_env(env_list);
	init_shell_variables(&env_list);
	setup_interactive_signals();
	return (env_list);
}

void	cleanup_env_array(char **env_array)
{
	int	i;

	if (!env_array)
		return ;
	i = 0;
	while (env_array[i])
	{
		env_gc_free(env_array[i]);
		i++;
	}
	env_gc_free(env_array);
}

int process_command_line(char *line, t_env **env_list, char **env)
{
    t_token *tokens;
    t_parser *cmd_list;
    char **updated_env;

    // Input'u temizle - başta ve sonda boşlukları trim et
    while (*line && (*line == ' ' || *line == '\t'))
        line++;
    
    if (!line || *line == '\0')
        return 0;

    add_history(line);
    gb_malloc(ft_strlen(line));
    
    tokens = tokenize_input(line);
    if (!tokens)
    {
        // Tokenize fail olursa signal'ları reset et
        setup_interactive_signals();
        return 0; // Shell devam etsin
    }
    
    cmd_list = parse_tokens(tokens, *env_list);
    if (!cmd_list)
    {
        // Parse fail olursa signal'ları reset et  
        setup_interactive_signals();
        return 0; // Shell devam etsin
    }
    
    // Debug: Command'ı kontrol et
    //if (cmd_list && cmd_list->argv && cmd_list->argv[0])
    //{
        //   printf("---\n");
        // Command execute et
        updated_env = env_list_to_array(*env_list);
        if (updated_env)
        {
            execute_cmds(cmd_list, updated_env, env_list);
            cleanup_env_array(updated_env);
        }
        else
            execute_cmds(cmd_list, env, env_list);
    //}

    // Execute sonrası signal'ları reset et
    setup_interactive_signals();
    
    return 0; // Shell devam etsin
}

void shell_loop(t_env *env_list, char **env)
{
    char *line;

    while (1)
    {
        // Her loop başında signal'ları garanti et
        //setup_interactive_signals();
        
        //// Readline state'ini temizle - ÇOK ÖNEMLİ!
        rl_on_new_line();
        rl_replace_line("", 0);
        
        line = readline("MiniShell->>>   ");
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (line[0] == '\0')
        {
            free(line);
            continue; // Boş satır, devam et
        }
        
        // Debug için line'ı kontrol et
        if (line && ft_strlen(line) > 0)
        {
            if (process_command_line(line, &env_list, env))
            {
                free(line);
                break; // Shell'den çık
            }
        }
        free(line);
        
        // Loop sonunda da readline state'ini temizle
        rl_on_new_line();
        rl_replace_line("", 0);
        setup_interactive_signals();
    }
}

int main(int ac, char **av, char **env)
{
    t_env *env_list;
    
    (void)ac;
    (void)av;
    
    set_last_exit_status(0);
      
    env_list = initialize_shell(env);
    shell_loop(env_list, env);
    
    rl_clear_history();
    env_gb_free_all();
    gb_free_all();
    
    return get_last_exit_status();
}