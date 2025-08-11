#include "shell.h"

static void	cleanup_env_array(char **env_array)
{
	int	i;

	if (!env_array)
		return ;
	i = 0;
	while (env_array[i])
	{
		env_gc_free(env_array[i]);
		i++;
	}
	env_gc_free(env_array);
}

static void	trim_spaces(char **line)
{
	while (**line && ft_ispace(**line))
		(*line)++;
}

int	process_command_line(char *line, t_env **env_list, char **env)
{
	t_token		*tokens;
	t_parser	*cmd_list;
	char		**updated_env;

	trim_spaces(&line);
	if (!*line)
		return (0);
	add_history(line);
	tokens = tokenize_input(line);
	if (!tokens)
		return (0);
	cmd_list = parse_tokens(tokens, *env_list);
	if (!cmd_list)
		return (0);
	if (cmd_list && cmd_list->argv && cmd_list->argv[0])
	{
		updated_env = env_list_to_array(*env_list);
		if (updated_env)
		{
			execute_cmds(cmd_list, updated_env, env_list);
			cleanup_env_array(updated_env);
		}
		else
			execute_cmds(cmd_list, env, env_list);
	}
	return (0);
}

void	shell_loop(t_env *env_list, char **env)
{
	char	*line;

	while (1)
	{
		setup_interactive_signals();
		line = readline("MiniShell->>>   ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (line[0] == '\0')
		{
			free(line);
			continue ;
		}
		if (process_command_line(line, &env_list, env))
		{
			free(line);
			break ;
		}
		free(line);
		setup_interactive_signals();
	}
}