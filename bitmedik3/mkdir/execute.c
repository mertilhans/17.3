#include "shell.h"

static int	data_init(t_exec_data *data, t_parser *cmd_list, char **env)
{
	data->original_stdin = dup(STDIN_FILENO);
	data->original_stdout = dup(STDOUT_FILENO);
	if (data->original_stdin == -1 || data->original_stdout == -1)
	{
		perror("dup failed");
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		return (1);
	}
	data->pids_size = count_commands(cmd_list);
	data->pids = NULL;
	data->i = 0;
	cmd_list->fd_i = data->original_stdin;
	cmd_list->fd_o = data->original_stdout;
	data->env = env;
	data->in_fd = STDIN_FILENO;
	data->pipefd[0] = -1;
	data->pipefd[1] = -1;
	return (0);
}

static int	handle_heredocs(t_parser *cmd_list, t_exec_data *data,
							t_env *env_list)
{
	t_parser	*current_cmd;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		current_cmd->heredoc_fd = process_heredocs(current_cmd, env_list);
		if (current_cmd->heredoc_fd == -1)
		{
			set_last_exit_status(130);
			if (data->original_stdin != -1)
			{
				dup2(data->original_stdin, STDIN_FILENO);
				close(data->original_stdin);
			}
			if (data->original_stdout != -1)
			{
				dup2(data->original_stdout, STDOUT_FILENO);
				close(data->original_stdout);
			}
			setup_interactive_signals();
			return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}

static int	handle_single_builtin(t_parser *cmd_list, t_exec_data *data,
								t_env **env_list)
{
	setup_interactive_signals();
	if (cmd_list->heredoc_fd != -1 && cmd_list->heredoc_fd != -2)
	{
		if (dup2(cmd_list->heredoc_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 heredoc for builtin");
			set_last_exit_status(1);
		}
		close(cmd_list->heredoc_fd);
		cmd_list->heredoc_fd = -1;
	}
	if (setup_file_redirections(cmd_list) != 0)
		set_last_exit_status(1);
	else
		ft_builtin_call(cmd_list, data, env_list);
	if (data->original_stdin != -1)
	{
		dup2(data->original_stdin, STDIN_FILENO);
		close(data->original_stdin);
	}
	if (data->original_stdout != -1)
	{
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
	}
	setup_interactive_signals();
	return (get_last_exit_status());
}

static void	finish_execution(t_parser *cmd_list, t_exec_data *data)
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
	dup2(data->original_stdin, STDIN_FILENO);
	close(data->original_stdin);
	dup2(data->original_stdout, STDOUT_FILENO);
	close(data->original_stdout);
}

int	execute_cmds(t_parser *cmd_list, char **env, t_env **env_list)
{
	t_exec_data	data;

	if (data_init(&data, cmd_list, env))
		return (1);
	if (handle_heredocs(cmd_list, &data, *env_list))
		return (get_last_exit_status());
	if (data.pids_size == 1 && is_builtin(cmd_list))
	{
		set_last_exit_status(handle_single_builtin(cmd_list, &data,
				env_list));
		return (get_last_exit_status());
	}
	setup_parent_execution_signals();
	data.pids = gb_malloc(sizeof(int) * data.pids_size);
	if (!data.pids)
	{
		perror("pids error");
		finish_execution(cmd_list, &data);
		setup_interactive_signals();
		return (1);
	}
	execute_loop(cmd_list, &data, env_list);
	wait_pids(&data);
	gc_free(data.pids);
	setup_interactive_signals();
	finish_execution(cmd_list, &data);
	return (get_last_exit_status());
}