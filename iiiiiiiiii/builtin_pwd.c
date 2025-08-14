#include "shell.h"

void	builtin_pwd(void)
{
	char	*cwd;
	size_t	len;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		len = ft_strlen(cwd);
		if (write(STDOUT_FILENO, cwd, len) == -1)
		{
			if (errno != EPIPE)
				perror("write");
			set_last_exit_status(1);
		}
		else
		{
			if (write(STDOUT_FILENO, "\n", 1) == -1)
			{
				if (errno != EPIPE)
					perror("write");
				set_last_exit_status(1);
			}
			else
				set_last_exit_status(0);
		}
		free(cwd);
	}
	else
	{
		perror("pwd");
		set_last_exit_status(1);
	}
}