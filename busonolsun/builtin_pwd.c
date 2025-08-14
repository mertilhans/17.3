#include "shell.h"

void	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		if (write(STDOUT_FILENO, cwd, ft_strlen(cwd)) == -1)
		{
			perror("write");
			set_last_exit_status(1);
		}
		else
		{
			if (write(STDOUT_FILENO, "\n", 1) == -1)
			{
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