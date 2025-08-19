/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:24 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 19:50:05 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

static int	handle_cd_errors(int arg_count, char **old_pwd_ptr)
{
	if (!*old_pwd_ptr)
		ft_putendl_fd("cd: error retrieving current directory: getcwd:"
			"cannot access parent directories:"
			"No such file or directory\n",
			2);
	if (arg_count > 2)
	{
		printf("bash: cd: too many arguments\n");
		if (*old_pwd_ptr)
		{
			free(*old_pwd_ptr);
			*old_pwd_ptr = NULL;
		}
		return (1);
	}
	return (0);
}

static int	go_home_directory(char **old_pwd_ptr)
{
	char	*home_dir;

	home_dir = getenv("HOME");
	if (home_dir == NULL)
	{
		printf("cd: HOME not set\n");
		if (*old_pwd_ptr)
		{
			free(*old_pwd_ptr);
			*old_pwd_ptr = NULL;
		}
		return (1);
	}
	if (chdir(home_dir) != 0)
	{
		if (*old_pwd_ptr)
		{
			free(*old_pwd_ptr);
			*old_pwd_ptr = NULL;
		}
		return (perror("cd"), 1);
	}
	return (0);
}

static int	change_directory(t_parser *cmd, int arg_count, char **old_pwd_ptr)
{
	*old_pwd_ptr = getcwd(NULL, 0);
	if (handle_cd_errors(arg_count, old_pwd_ptr) != 0)
		return (1);
	if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
	{
		if (go_home_directory(old_pwd_ptr) != 0)
			return (1);
	}
	else if (chdir(cmd->argv[1]) != 0)
	{
		if (*old_pwd_ptr)
		{
			free(*old_pwd_ptr);
			*old_pwd_ptr = NULL;
		}
		return (perror("cd"), 1);
	}
	return (0);
}

static void	update_and_free_pwd(char *old_pwd, t_env **env_list,
		t_export **export_list)
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
		free(new_pwd);
	}
	if (old_pwd)
		free(old_pwd);
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
		free(old_pwd);
	if (current_pwd_for_free)
		free(current_pwd_for_free);
	set_last_exit_status(exit_code);
	return (exit_code);
}
