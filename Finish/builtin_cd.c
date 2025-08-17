/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:24 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 18:12:01 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>
static int	change_directory(t_parser *cmd, int arg_count, char **old_pwd_ptr)
{
	char	*home_dir;

	*old_pwd_ptr = getcwd(NULL, 0);
	if (arg_count > 2)
	{
		printf("bash: cd: too many arguments\n");
		return (1);
	}
	else if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
	{
		home_dir = getenv("HOME");
		if (home_dir == NULL)
		{
			printf("cd: HOME not set\n");
			return (1);
		}
		if (chdir(home_dir) != 0)
			return (perror("cd"), 1);
	}
	else if (chdir(cmd->argv[1]) != 0)
		return (perror("cd"), 1);
	return (0);
}

static void	update_and_free_pwd(char *old_pwd, t_env **env_list, t_export **export_list)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		set_env_value(env_list, "PWD", new_pwd);
		set_export_value(export_list, "PWD", new_pwd);
		if (old_pwd)
		{
			set_env_value(env_list, "OLDPWD", old_pwd);
			set_export_value(export_list, "OLDPWD", old_pwd);
		}
		env_gc_free(new_pwd);
	}
	if (old_pwd)
		env_gc_free(old_pwd);
}

int	built_cd(t_parser *cmd, t_env **env_list, t_export **export_list)
{
	char	*old_pwd;
	char	*current_pwd_for_free;
	int		arg_count;
	int		exit_code;

	current_pwd_for_free = getcwd(NULL, 0);
	arg_count = 0;
	while (cmd->argv[arg_count])
		arg_count++;
	exit_code = change_directory(cmd, arg_count, &old_pwd);
	if (exit_code == 0)
		update_and_free_pwd(old_pwd, env_list, export_list);
	else if (old_pwd)
		env_gc_free(old_pwd);
	if (current_pwd_for_free)
		env_gc_free(current_pwd_for_free);
	set_last_exit_status(exit_code);
	return (exit_code);
}
