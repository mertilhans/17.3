/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 18:33:54 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/19 13:04:12 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>

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
	{
		set_env_value(env_list, "OLDPWD", NULL);
		set_export_value(export_list, "OLDPWD", NULL);
	}
}

void	init_shell_variables(t_env **env_list)
{
	t_export	**export_list;

	export_list = get_export_list();
	init_shlvl(env_list, export_list);
	init_pwd(env_list, export_list);
	init_oldpwd(env_list, export_list);
}
