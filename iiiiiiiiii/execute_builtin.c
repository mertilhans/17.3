	#include "shell.h"

	int ft_builtin_call(t_parser *cmd, t_exec_data *data, t_env **env_list)
	{
		if (cmd && cmd->argv && cmd->argv[0])
			printf("[DEBUG] ft_builtin_call: %s\n", cmd->argv[0]);
		// ...existing code...
#include "shell.h"

int	is_builtin(t_parser *cmd)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (ft_strcmp(cmd->argv[0], "exit") == 0
		|| ft_strcmp(cmd->argv[0], "cd") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "pwd") == 0
		|| ft_strcmp(cmd->argv[0], "echo") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "export") == 0
		|| ft_strcmp(cmd->argv[0], "unset") == 0)
		return (1);
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (1);
	return (0);
}

static int	ft_builtin_call_2(t_parser *cmd, t_env **env_list)
{
	if (ft_strcmp(cmd->argv[0], "export") == 0)
	{
		builtin_export(cmd, env_list);
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
	{
		builtin_pwd();
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
	{
		builtin_unset(cmd, env_list);
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "env") == 0)
	{
		if (cmd->argv[1])
		{
			ft_putendl_fd(" : No such file or directory", 2);
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

	if (ft_strcmp(cmd->argv[0], "echo") == 0)
	{
		built_echo(cmd);
		return (get_last_exit_status());
	}
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (built_cd(cmd));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
	{
		cmd->fd_i = data->original_stdin;
		cmd->fd_o = data->original_stdout;
		builtin_exit(cmd);
		return (0);
	}
	result = ft_builtin_call_2(cmd, env_list);
	if (result != -1)
		return (result);
	return (-1);
}