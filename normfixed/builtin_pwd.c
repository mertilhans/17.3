/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:36 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 15:21:53 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	handle_pwd_write_error(void)
{
	perror("write");
	set_last_exit_status(1);
}

static void	print_pwd(const char *cwd)
{
	if (write(STDOUT_FILENO, cwd, ft_strlen(cwd)) == -1)
		handle_pwd_write_error();
	else if (write(STDOUT_FILENO, "\n", 1) == -1)
		handle_pwd_write_error();
	else
		set_last_exit_status(0);
}

void	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		print_pwd(cwd);
		free(cwd);
	}
	else
	{
		perror("pwd");
		set_last_exit_status(1);
	}
}
