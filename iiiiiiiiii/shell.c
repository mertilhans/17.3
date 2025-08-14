#include "shell.h"

int	main(int ac, char **av, char **env)
{
	t_env	*env_list;

	(void)ac;
	(void)av;
	set_last_exit_status(0);
	env_list = initialize_shell(env);
	shell_loop(env_list, env);
	rl_clear_history();
	env_gb_free_all();
	gb_free_all();
	return (get_last_exit_status());
}