#include "shell.h"

static int	handle_echo_cd(t_parser *cmd)
{
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
	{
		built_echo(cmd);
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (built_cd(cmd));
	return (-1);
}

static int	handle_exit(t_parser *cmd, t_exec_data *data)
{
	cmd->fd_i = data->original_stdin;
	cmd->fd_o = data->original_stdout;
	builtin_exit(cmd);
	return (0);
}

static int	handle_pwd_export_unset(t_parser *cmd, t_env **env_list)
{
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
	{
		builtin_pwd();
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "export") == 0)
	{
		builtin_export(cmd, env_list);
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
	{
		builtin_unset(cmd, env_list);
		return (get_last_exit_status());
	}
	return (-1);
}

static int	handle_env(t_parser *cmd, t_env **env_list)
{
	if (ft_strcmp(cmd->argv[0], "env") == 0)
	{
		if (cmd->argv[1])
		{
			ft_putendl_fd(" : No such file or directory\n", 2);
			set_last_exit_status(127);
			return (127);
		}
		builtin_env(*env_list);
		return (0);
	}
	return (-1);
}

int	ft_builtin_call(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	result;

	result = handle_echo_cd(cmd);
	if (result != -1)
		return (result);
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (handle_exit(cmd, data));
	result = handle_pwd_export_unset(cmd, env_list);
	if (result != -1)
		return (result);
	result = handle_env(cmd, env_list);
	if (result != -1)
		return (result);
	return (-1);
}