/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 04:15:36 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/25 19:39:45 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

static int	handle_line_input(char *line, t_env **env_list, char **env)
{
	char	*res;

	// readline NULL döndürdüğünde: ya Ctrl+D'dir ya da Ctrl+C
	if (!line)
	{
		// Eğer global değişkenimiz sinyal geldiyse bunun Ctrl+C olduğunu anlarız
		if (g_signal_status == SIGINT)
		{
			g_signal_status = 0; // Sinyali işledik, sıfırla
			return (2);          // Döngüye devam et, yeni prompt basılsın
		}
		// Sinyal yoksa, bu kesinlikle Ctrl+D'dir (EOF).
		write(STDOUT_FILENO, "exit\n", 5);
		return (1); // Döngüyü kır ve çık
	}
	res = ft_strdup(line);
	free(line);
	if (!res)
	{
		write(STDOUT_FILENO, "exit\n", 5);
		return (1);
	}
	if (res[0] == '\0')
	{
		gc_free(res);
		return (2);
	}
	if (ft_strlen(res) > 0)
		process_command_line(res, env_list, env);
	gc_free(res);
	return (0);
}

void	shell_loop(t_env *env_list, char **env)
{
	char	*line;
	int		status;

	while (1)
	{
		ft_init_signals(PROMPT);
		line = readline("MiniShell->>>   ");
		status = handle_line_input(line, &env_list, env);
		if (status == 1)
			break ;
		else if (status == 2)
			continue ;
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
	return (0);
}