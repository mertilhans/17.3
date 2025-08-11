#include "shell.h"

static int	check_n_flag(char *arg)
{
	int	j;
	int	valid_flag;

	j = 2;
	valid_flag = 1;
	while (arg[j])
	{
		if (arg[j] != 'n')
		{
			valid_flag = 0;
			break ;
		}
		j++;
	}
	return (valid_flag);
}

static int	process_n_flags(t_parser *cmd, int *i)
{
	int	newline;

	newline = 1;
	while (cmd->argv[*i] && cmd->argv[*i][0] == '-' && cmd->argv[*i][1] == 'n')
	{
		if (check_n_flag(cmd->argv[*i]))
		{
			newline = 0;
			(*i)++;
		}
		else
			break ;
	}
	return (newline);
}

void	built_echo(t_parser *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = process_n_flags(cmd, &i);
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