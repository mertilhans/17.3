/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 04:15:36 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 23:38:31 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	handle_line_input(char *line, t_env **env_list, char **env)
{
	if (!line)
	{
		printf("exit\n");
		return (1);
	}
	if (line[0] == '\0')
	{
		free(line);
		return (2);
	}
	if (ft_strlen(line) > 0)
	{
		if (process_command_line(line, env_list, env))
		{
			free(line);
			return (1);
		}
	}
	return (0);
}

void	shell_loop(t_env *env_list, char **env)
{
	char	*line;
	int		status;

	while (1)
	{
		if(get_last_exit_status() == 130)
			write(1,"\n",1);
		rl_on_new_line();
		rl_replace_line("", 0);
		line = readline("MiniShell->>>   ");
		status = handle_line_input(line, &env_list, env);
		if (status == 1)
			break ;
		else if (status == 2)
			continue ;
		setup_interactive_signals();
	}
}

int	process_command_line(char *line, t_env **env_list, char **env)
{
	t_parser	*cmd_list;
	char		**updated_env;

	while (*line && (*line == ' ' || *line == '\t'))
		line++;
	if (!line || *line == '\0')
		return (0);
	add_history(line);
	cmd_list = parse_line(line, *env_list);
	if (!cmd_list)
		return (0);
	updated_env = env_list_to_array(*env_list);
	if (updated_env)
	{
		execute_cmds(cmd_list, updated_env, env_list);
		cleanup_env_array(updated_env);
	}
	else
		execute_cmds(cmd_list, env, env_list);
	setup_interactive_signals();
	return (0);
}
