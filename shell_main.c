/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 04:14:54 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/25 19:05:51 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <readline/readline.h>

t_env	*initialize_shell(char **env)
{
	t_env		*env_list;
	t_export	**export_list;

	env_list = init_env(env);
	export_list = get_export_list();
	*export_list = init_export_from_env(env_list);
	init_shell_variables(&env_list);
	ft_init_signals(PROMPT);
	return (env_list);
}

int	main(int ac, char **av, char **env)
{
	t_env	*env_list;

	if (ac != 1)
	{
		ft_putendl_fd("No usage syntax\n", 2);
		return (1);
	}
	(void)av;
	set_last_exit_status(0);
	env_list = initialize_shell(env);
	shell_loop(env_list, env);
	rl_clear_history();
	env_gb_free_all();
	gb_free_all();
	return (get_last_exit_status());
}
