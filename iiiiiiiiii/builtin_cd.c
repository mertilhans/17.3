#include "shell.h"

static int	count_args(char **argv)
{
	int	count;

	count = 0;
	while (argv[count])
		count++;
	return (count);
}

static int	change_to_home(void)
{
	char	*home_dir;

	home_dir = getenv("HOME");
	if (home_dir == NULL)
	{
		printf("cd: HOME not set\n");
		return (1);
	}
	if (chdir(home_dir) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

static void	update_pwd_vars(char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		setenv("PWD", new_pwd, 1);
		if (old_pwd)
			setenv("OLDPWD", old_pwd, 1);
		free(new_pwd);
	}
}

int	built_cd(t_parser *cmd)
{
	char	*current_pwd;
	char	*old_pwd;
	int		arg_count;
	int		exit_code;

	current_pwd = getcwd(NULL, 0);
	old_pwd = current_pwd ? ft_strdup(current_pwd) : NULL;
	arg_count = count_args(cmd->argv);
	exit_code = 0;
	if (arg_count > 2)
	{
		printf("bash: cd: too many arguments\n");
		exit_code = 1;
	}
	else if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
		exit_code = change_to_home();
	else if (chdir(cmd->argv[1]) != 0)
	{
		perror("cd");
		exit_code = 1;
	}
	if (exit_code == 0)
		update_pwd_vars(old_pwd);
	if (current_pwd)
		free(current_pwd);
	if (old_pwd)
		gc_free(old_pwd);
	set_last_exit_status(exit_code);
	return (exit_code);
}