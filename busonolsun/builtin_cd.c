#include "shell.h"
static int	change_directory(t_parser *cmd, int arg_count, char **old_pwd_ptr)
{
	char	*home_dir;

	*old_pwd_ptr = getcwd(NULL, 0);
	if (arg_count > 2)
	{
		printf("bash: cd: too many arguments\n");
		return (1);
	}
	else if (cmd->argv[1] == NULL || cmd->argv[1][0] == '\0')
	{
		home_dir = getenv("HOME");
		if (home_dir == NULL)
		{
			printf("cd: HOME not set\n");
			return (1);
		}
		if (chdir(home_dir) != 0)
			return (perror("cd"), 1);
	}
	else if (chdir(cmd->argv[1]) != 0)
		return (perror("cd"), 1);
	return (0);
}

static void	update_and_free_pwd(char *old_pwd)
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
	if (old_pwd)
		gc_free(old_pwd);
}

int	built_cd(t_parser *cmd)
{
	char	*old_pwd;
	char	*current_pwd_for_free;
	int		arg_count;
	int		exit_code;

	current_pwd_for_free = getcwd(NULL, 0);
	arg_count = 0;
	while (cmd->argv[arg_count])
		arg_count++;
	exit_code = change_directory(cmd, arg_count, &old_pwd);
	if (exit_code == 0)
		update_and_free_pwd(old_pwd);
	else if (old_pwd)
		gc_free(old_pwd);
	if (current_pwd_for_free)
		free(current_pwd_for_free);
	set_last_exit_status(exit_code);
	return (exit_code);
}