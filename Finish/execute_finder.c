/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_finder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:02 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:03 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char	*find_executable(char *cmd, t_env *env_list)
{
	char	*path;

	if (!cmd || !*cmd)
		return (NULL);
	if (is_exec_other(cmd))
		return (ft_strdup(cmd));
	path = get_env_value(env_list, "PATH");
	if (!path)
		return (NULL);
	return (search_in_path(cmd, path));
}

char	*search_in_path(char *cmd, char *path)
{
	char	*start;
	char	*finish;
	char	*found;
	int		len;

	start = path;
	finish = path;
	while (1)
	{
		if (*start == ':' || *start == '\0')
		{
			len = start - finish;
			if (len > 0)
			{
				found = check_cmd_in_dir(finish, len, cmd);
				if (found)
					return (found);
			}
			if (*start == '\0')
				break ;
			finish = start + 1;
		}
		start++;
	}
	return (NULL);
}

char	*check_cmd_in_dir(const char *dir_start, int len, char *cmd)
{
	char	*result;
	char	*dir;

	dir = copy_dir(dir_start, len);
	if (!dir)
		return (NULL);
	result = join_path(dir, cmd);
	if (is_exec_other(result))
		return (result);
	return (NULL);
}

int	is_exec_other(char *path)
{
	struct stat	path_stat;

	if (access(path, F_OK) != 0)
		return (0);
	if (stat(path, &path_stat) != 0)
		return (0);
	if (S_ISDIR(path_stat.st_mode))
		return (0);
	if (access(path, X_OK) != 0)
		return (0);
	return (1);
}

char	*join_path(char *dir, char *file)
{
	size_t	dlen;
	size_t	flen;
	char	*res;

	flen = strlen(file);
	dlen = strlen(dir);
	res = gb_malloc(dlen + flen + 2);
	if (!res)
		return (NULL);
	ft_memcpy(res, dir, dlen);
	res[dlen] = '/';
	ft_memcpy(res + dlen + 1, file, flen);
	res[dlen + flen + 1] = '\0';
	return (res);
}