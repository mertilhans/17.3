#include "shell.h"

static int	check_n_flag(char *arg)
{
	int	j;

	if (arg[0] != '-' || arg[1] != 'n')
		return (0);
	j = 2;
	while (arg[j])
	{
		if (arg[j] != 'n')
			return (0);
		j++;
	}
	return (1);
}

void	built_echo(t_parser *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (cmd->argv[i] && check_n_flag(cmd->argv[i]))
	{
		newline = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		write(STDOUT_FILENO, cmd->argv[i], ft_strlen(cmd->argv[i]));
		if (cmd->argv[i + 1] != NULL)
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	set_last_exit_status(0);
}