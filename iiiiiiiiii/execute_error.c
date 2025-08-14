#include "shell.h"

void	ft_putendl_fd(char *str, int fd)
{
	if (!str)
		return ;
	while (*str)
		write(fd, str++, 1);
}

void	ft_directory(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": is a directory\n", 2);
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(126);
}

void	ft_not_directory(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": No such file or directory\n", 2);
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(127);
}

void	ft_permission_denied(t_parser *cmd, t_exec_data *data)
{
	ft_putendl_fd("bash: ", 2);
	ft_putendl_fd(cmd->argv[0], 2);
	ft_putendl_fd(": Permission denied\n", 2);
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(126);
}

void	ft_not_executable(t_parser *cmd, t_exec_data *data,
	char *exec_path)
{
	if (!exec_path)
	{
		ft_putendl_fd("bash: ", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found\n", 2);
	}
	else
	{
		ft_putendl_fd("bash: ", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd(": Permission denied\n", 2);
	}
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(127);
}
