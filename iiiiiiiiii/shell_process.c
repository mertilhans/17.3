#include "shell.h"

static char	*trim_line(char *line)
{
	while (*line && (*line == ' ' || *line == '\t'))
		line++;
	return (line);
}

static int	execute_parsed_commands(t_parser *cmd_list, t_env **env_list,
	char **env)
{
	char	**updated_env;

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

static t_parser	*parse_and_check(char *trimmed, t_env *env_list)
{
	t_token		*tokens;
	t_parser	*cmd_list;

	tokens = tokenize_input(trimmed);
	if (!tokens)
	{
		setup_interactive_signals();
		return (NULL);
	}
	cmd_list = parse_tokens(tokens, env_list);
	if (!cmd_list)
	{
		setup_interactive_signals();
		return (NULL);
	}
	return (cmd_list);
}

int	process_command_line(char *line, t_env **env_list, char **env)
{
	char		*trimmed;
	t_parser	*cmd_list;

	trimmed = trim_line(line);
	if (!trimmed || *trimmed == '\0')
		return (0);
	add_history(line);
	gb_malloc(ft_strlen(line));
	cmd_list = parse_and_check(trimmed, *env_list);
	if (!cmd_list)
		return (0);
	execute_parsed_commands(cmd_list, env_list, env);
	return (0);
}