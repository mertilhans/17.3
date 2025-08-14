#include "shell.h"

static void	handle_builtin_exit(t_parser *cmd, t_exec_data *data,
	t_env **env_list)
{
	int	exit_code;

	exit_code = ft_builtin_call(cmd, data, env_list);
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(exit_code);
}

static void	setup_child_pipes(t_exec_data *data, t_parser *cmd)
{
	if (cmd->heredoc_fd == -1 || cmd->heredoc_fd == -2)
	{
		if (data->in_fd != STDIN_FILENO)
		{
			if (dup2(data->in_fd, STDIN_FILENO) == -1)
			{
				perror("dup2 in_fd");
				exit(1);
			}
			close(data->in_fd);
		}
	}
	else if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
	if (cmd->next && data->pipefd[1] != -1)
	{
		if (dup2(data->pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 pipefd[1]");
			exit(1);
		}
		close(data->pipefd[0]);
		close(data->pipefd[1]);
	}
}

static void	handle_slash_command(t_parser *cmd, t_exec_data *data,
	struct stat *path_stat)
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
}

static void	handle_no_slash_command(t_parser *cmd, t_exec_data *data)
{
	char	*exec_path;

	if (access(cmd->argv[0], F_OK) == 0)
	{
		ft_putendl_fd("bash: ", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found\n", 2);
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		close_all_fds_except_std(cmd);
		gb_free_all();
		env_gb_free_all();
		exit(127);
	}
	exec_path = find_executable(cmd->argv[0]);
	if (!exec_path)
		ft_not_executable(cmd, data, NULL);
	if (access(exec_path, X_OK) != 0)
		ft_permission_denied(cmd, data);
	execve(exec_path, cmd->argv, data->env);
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(127);
}

void	child_process_exec(t_parser *cmd, t_exec_data *data,
	t_env **env_list)
{
	struct stat	path_stat;

	setup_child_signals();
	if (data->in_fd != STDIN_FILENO)
	{
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
	}
	setup_child_pipes(data, cmd);
	if (setup_file_redirections(cmd) == -1)
	{
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		close_all_fds_except_std(cmd);
		env_gb_free_all();
		gb_free_all();
		exit(1);
	}
	if (!cmd->argv[0] || cmd->argv[0][0] == '\0')
	{
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		close_all_fds_except_std(cmd);
		gb_free_all();
		env_gb_free_all();
		exit(0);
	}
	if (is_builtin(cmd))
		handle_builtin_exit(cmd, data, env_list);
	if (ft_strchr(cmd->argv[0], '/'))
		handle_slash_command(cmd, data, &path_stat);
	else
		handle_no_slash_command(cmd, data);
}
