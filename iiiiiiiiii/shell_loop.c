#include "shell.h"

void	cleanup_env_array(char **env_array)
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

static void	handle_line_input(char *line, t_env **env_list, char **env)
{
	if (ft_strlen(line) > 0)
	{
		if (process_command_line(line, env_list, env))
		{
			free(line);
			return ;
		}
	}
}

void	shell_loop(t_env *env_list, char **env)
{
	char	*line;

	while (1)
	{
		rl_on_new_line();
		rl_replace_line("", 0);
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
		handle_line_input(line, &env_list, env);
		free(line);
	}
}

