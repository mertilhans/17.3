#include "shell.h"

void	execute_loop(t_parser *cmd_list, t_exec_data *data, t_env **env_list)
{
	t_parser	*cmd;

	data->in_fd = STDIN_FILENO;
	cmd = cmd_list;
	while (cmd)
	{
		if (!process_command(cmd, data, env_list))
			break ;
		cmd = cmd->next;
	}
	if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
}

void	wait_pids(t_exec_data *data)
{
	int	status;
	int	j;

	j = 0;
	while (j < data->i)
	{
		waitpid(data->pids[j], &status, 0);
		if (j == data->i - 1)
			set_last_exit_status(calculate_exit_status(status));
		j++;
	}
}

int	count_commands(t_parser *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

static int	setup_pipe_if_needed(t_parser *cmd, t_exec_data *data)
{
	if (!cmd->next)
		return (0);
	if (pipe(data->pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

int	process_command(t_parser *cmd, t_exec_data *data, t_env **env_list)
{
	int	pid;

	data->pipefd[0] = -1;
	data->pipefd[1] = -1;
	if (setup_pipe_if_needed(cmd, data) == -1)
		return (0);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (cmd->next)
		{
			close(data->pipefd[0]);
			close(data->pipefd[1]);
		}
		return (0);
	}
	if (pid == 0)
		child_process_exec(cmd, data, env_list);
	if (data->i >= data->pids_size)
	{
		printf("limit error\n");
		return (0);
	}
	data->pids[data->i++] = pid;
	if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != -2)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	if (data->in_fd != STDIN_FILENO)
		close(data->in_fd);
	if (cmd->next)
	{
		if (data->pipefd[0] != -1)
		{
			close(data->pipefd[1]);
			data->in_fd = data->pipefd[0];
		}
		else
			data->in_fd = STDIN_FILENO;
	}
	else
		data->in_fd = STDIN_FILENO;
	return (1);
}