#include "shell.h"
static int	process_n_flags(t_parser *cmd, int *newline)
{
	int	i;
	int	j;
	int	valid_flag;

	i = 1;
	while (cmd->argv[i] && cmd->argv[i][0] == '-' && cmd->argv[i][1] == 'n')
	{
		j = 2;
		valid_flag = 1;
		while (cmd->argv[i][j])
		{
			if (cmd->argv[i][j] != 'n')
			{
				valid_flag = 0;
				break ;
			}
			j++;
		}
		if (valid_flag)
			*newline = 0;
		else
			break ;
		i++;
	}
	return (i);
}

void	built_echo(t_parser *cmd)
{
	int	i;
	int	newline;

	newline = 1;
	i = process_n_flags(cmd, &newline);
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