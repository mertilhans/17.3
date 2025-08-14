#include "shell.h"

static void	execute_loop(t_parser *cmd_list, t_exec_data *data,
	t_env **env_list)
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

static void	wait_pids(t_exec_data *data)
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

void	execute_pipeline(t_parser *cmd_list, t_exec_data *data,
	t_env **env_list)
{
	data->pids = gb_malloc(sizeof(int) * data->pids_size);
	if (!data->pids)
	{
		perror("pids error\n");
		restore_fds(data);
		return ;
	}
	execute_loop(cmd_list, data, env_list);
	wait_pids(data);
	gc_free(data->pids);
}

void	finish_execution(t_parser *cmd_list, t_exec_data *data)
{
	t_parser	*current_cmd;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		if (current_cmd->heredoc_fd != -1 && current_cmd->heredoc_fd != -2)
		{
			close(current_cmd->heredoc_fd);
			current_cmd->heredoc_fd = -1;
		}
		current_cmd = current_cmd->next;
	}
	restore_fds(data);
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