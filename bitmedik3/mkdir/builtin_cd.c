#include "shell.h"

static int	update_pwd_vars(char *old_pwd)
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
	return (0);
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

static int	check_arg_count(t_parser *cmd)
{
	int	arg_count;

	arg_count = 0;
	while (cmd->argv[arg_count])
		arg_count++;
	if (arg_count > 2)
	{
		printf("bash: cd: too many arguments\n");
		return (1);
	}
	return (0);
}

int	built_cd(t_parser *cmd)
{
	char	*current_pwd;
	char	*old_pwd;
	int		exit_code;

	current_pwd = getcwd(NULL, 0);
	old_pwd = NULL;
	if (current_pwd)
		old_pwd = ft_strdup(current_pwd);
	exit_code = 0;
	if (check_arg_count(cmd))
		exit_code = 1;
	else if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
		exit_code = change_to_home();
	else
	{
		if (chdir(cmd->argv[1]) != 0)
		{
			perror("cd");
			exit_code = 1;
		}
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
