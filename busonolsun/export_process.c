#include "shell.h"

static void	assign_value(t_parser *cmd, int *i, char *key, char *value, t_env **env_list)
{
	t_export	**export_list;
	char		*full_value;

	export_list = get_export_list();
	if (cmd->argv[*i + 1])
	{
		full_value = export_build_value(cmd, i, value);
		set_export_value(export_list, key, full_value);
		set_env_value(env_list, key, full_value);
		free(full_value);
	}
	else
	{
		set_export_value(export_list, key, value);
		set_env_value(env_list, key, value);
	}
}

void	export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
{
	t_export	**export_list;
	char		*current_arg;
	char		*eq_pos;
	char		*key;
	char		*value;

	export_list = get_export_list();
	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	*eq_pos = '\0';
	key = current_arg;
	value = eq_pos + 1;
	if (ft_strlen(value) > 0)
		assign_value(cmd, i, key, value, env_list);
	else
	{
		set_export_value(export_list, key, "");
		set_env_value(env_list, key, "");
	}
	*eq_pos = '=';
}