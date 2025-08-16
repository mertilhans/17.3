#include "shell.h"

void	child_process_exec(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	setup_child_signals();
	if (data->in_fd != STDIN_FILENO)
		close_fd(data);
	if (cmd->heredoc_fd == -1 || cmd->heredoc_fd == -2)
		fd_not_stdin(data);
	else
	{
		if (data->in_fd != STDIN_FILENO)
			close(data->in_fd);
	}
	if (cmd->next && data->pipefd[1] != -1)
		next_cmd_and_fd(data);
	if (ft_redir_ctrl(cmd) == -1)
	{
		ft_free_and_close(data, cmd);
		exit(1);
	}
	ft_exec_start(cmd, data, env_list);
}

void	ft_exec_start(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	struct stat	path_stat;
	char		*exec_path;

	if (!cmd->argv[0] || cmd->argv[0][0] == '\0')
	{
		ft_free_and_close(data, cmd);
		exit(0);
	}
	if (is_builtin(cmd))
	{
		set_last_exit_status(ft_builtin_call(cmd, data, env_list));
		ft_free_and_close(data, cmd);
		exit(get_last_exit_status());
	}
	ft_exec_ctrl(cmd, data, &exec_path, &path_stat);
	exec_path = find_executable(cmd->argv[0], *env_list);
	if (!exec_path)
		ft_not_executable(cmd, data, NULL);
	if (access(exec_path, X_OK) != 0)
		ft_permission_denied(cmd, data);
	execve(exec_path, cmd->argv, data->env);
	ft_free_and_close(data, cmd);
	exit(127);
}

void	ft_exec_ctrl(t_parser *cmd, t_exec_data *data, char **exec_path,
		struct stat *path_stat)
{
	if (ft_strchr(cmd->argv[0], '/'))
	{
		if (stat(cmd->argv[0], path_stat) == 0)
		{
			if (S_ISDIR(path_stat->st_mode))
				ft_directory(cmd, data);
			if (access(cmd->argv[0], X_OK) != 0)
				ft_permission_denied(cmd, data);
		}
		else
			ft_not_directory(cmd, data);
		*exec_path = cmd->argv[0];
	}
	else
	{
		if (access(cmd->argv[0], F_OK) == 0)
		{
			ft_putendl_fd("bash: ", 2);
			ft_putendl_fd(cmd->argv[0], 2);
			ft_putendl_fd(": command not found\n", 2);
			ft_free_and_close(data, cmd);
			exit(127);
		}
	}
}

void	fd_not_stdin(t_exec_data *data)
{
	if (dup2(data->in_fd, STDIN_FILENO) == -1)
	{
		perror("dup2 in_fd");
		exit(1);
	}
	if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
}

void	next_cmd_and_fd(t_exec_data *data)
{
	if (dup2(data->pipefd[1], STDOUT_FILENO) == -1)
	{
		perror("dup2 pipefd[1]");
		exit(1);
	}
	close(data->pipefd[0]);
	close(data->pipefd[1]);
}