#include "shell.h"

int	is_numeric_string(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	if (str[0] == '+' || str[0] == '-')
		i = 1;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static void	cleanup_before_exit(t_parser *cmd)
{
	if (cmd->fd_i != -1)
		close(cmd->fd_i);
	if (cmd->fd_o != -1)
		close(cmd->fd_o);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	rl_clear_history();
}

void	builtin_exit(t_parser *cmd)
{
	int	exit_code;

	exit_code = 0;
	printf("exit\n");
	if (cmd->argv[1] && cmd->argv[2])
	{
		printf("bash: exit: too many arguments\n");
		set_last_exit_status(1);
		return ;
	}
	if (cmd->argv[1])
	{
		if (!is_numeric_string(cmd->argv[1]))
		{
			printf("bash: exit: %s: numeric argument required\n", cmd->argv[1]);
			exit_code = 2;
		}
		else
		{
			exit_code = ft_atoi(cmd->argv[1]);
			exit_code = ((exit_code % 256) + 256) % 256;
		}
	}
	else
		exit_code = get_last_exit_status();
	cleanup_before_exit(cmd);
	exit(exit_code);
}