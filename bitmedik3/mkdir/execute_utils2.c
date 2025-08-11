#include "shell.h"

static char	*copy_dir(const char *dir, int len)
{
	char	*res;
	int		i;

	i = 0;
	res = gb_malloc(len + 1);
	if (!res)
		return (NULL);
	while (i < len)
	{
		res[i] = dir[i];
		i++;
	}
	res[len] = '\0';
	return (res);
}

static char	*join_path(char *dir, char *file)
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

static int	is_exec_other(char *path)
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

static char	*check_cmd_in_dir(const char *dir_start, int len, char *cmd)
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

char	*find_executable(char *cmd)
{
	char	*path;
	char	*start;
	char	*finish;
	char	*found;
	int		len;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (is_exec_other(cmd))
			return (ft_strdup(cmd));
		return (NULL);
	}
	path = getenv("PATH");
	if (!path || !*path)
		return (NULL);
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