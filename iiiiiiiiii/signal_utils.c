#include "shell.h"

int	calculate_exit_status(int status)
{
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			return (130);
		else if (sig == SIGQUIT)
			return (131);
		else
			return (128 + sig);
	}
	return (1);
}