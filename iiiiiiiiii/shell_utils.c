// #include "shell.h"

// static void	execute_parsed_commands(t_parser *cmd_list, t_env **env_list,
// 	char **env)
// {
// 	char	**updated_env;

// 	updated_env = env_list_to_array(*env_list);
// 	if (updated_env)
// 	{
// 		execute_cmds(cmd_list, updated_env, env_list);
// 		cleanup_env_array(updated_env);
// 	}
// 	else
// 		execute_cmds(cmd_list, env, env_list);
// 	setup_interactive_signals();
// }