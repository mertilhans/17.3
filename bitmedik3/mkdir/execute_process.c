#include "shell.h"

static void	setup_child_fds(t_parser *cmd, t_exec_data *data)
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
	else
	{
		if (data->in_fd != STDIN_FILENO)
			close(data->in_fd);
	}
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

static void	handle_redirections(t_parser *cmd)
{
	if (ft_redir_ctrl(cmd) == -1)
	{
		close_all_fds_except_std(cmd);
		env_gb_free_all();
		gb_free_all();
		exit(1);
	}
}

static void	cleanup_and_exit(t_parser *cmd, t_exec_data *data, int code)
{
	if (data->original_stdin != -1)
		close(data->original_stdin);
	if (data->original_stdout != -1)
		close(data->original_stdout);
	close_all_fds_except_std(cmd);
	gb_free_all();
	env_gb_free_all();
	exit(code);
}

void	child_process_exec(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	setup_child_signals();
	if (data->in_fd != STDIN_FILENO)
	{
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
	}
	setup_child_fds(cmd, data);
	handle_redirections(cmd);
	ft_exec_start(cmd, data, env_list);
}

void	ft_exec_start(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	char			*exec_path;
	struct stat		path_stat;

	signal(SIGPIPE, SIG_IGN);
	if (is_builtin(cmd))
	{
		cleanup_and_exit(cmd, data, ft_builtin_call(cmd, data, env_list));
	}
	if (stat(cmd->argv[0], &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
	{
		ft_putendl_fd("bash :", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd("is a directory\n", 2);
		cleanup_and_exit(cmd, data, 126);
	}
	else if (ft_strchr(cmd->argv[0], '/') && access(cmd->argv[0], F_OK) != 0)
	{
		ft_putendl_fd("bash :", 2);
		ft_putendl_fd(cmd->argv[0], 2);
		ft_putendl_fd("No such file or directory\n", 2);
		cleanup_and_exit(cmd, data, 1);
	}
	exec_path = find_executable(cmd->argv[0]);
	if (!exec_path || access(exec_path, X_OK) != 0)
	{
		if (exec_path && access(exec_path, F_OK) == 0)
		{
			ft_putendl_fd("bash :", 2);
			ft_putendl_fd(cmd->argv[0], 2);
			ft_putendl_fd("Permission denied\n", 2);
			cleanup_and_exit(cmd, data, 126);
		}
		else
		{
			ft_putendl_fd(cmd->argv[0], 2);
			ft_putendl_fd(": command not found\n", 2);
			cleanup_and_exit(cmd, data, 127);
		}
	}
	signal(SIGPIPE, SIG_DFL);
	execve(exec_path, cmd->argv, data->env);
	perror("execve");
	cleanup_and_exit(cmd, data, 126);
}