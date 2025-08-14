#include "shell.h"

static int	is_valid_unset_identifier(char *identifier)
{
	if (identifier[0] == '$'
		|| !((identifier[0] >= 'a' && identifier[0] <= 'z')
			|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
			|| identifier[0] == '_'))
		return (0);
	return (1);
}

void	builtin_unset(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	int			has_error;
	char		*identifier;

	i = 1;
	export_list = get_export_list();
	has_error = 0;
	if (!cmd->argv[1])
	{
		printf("unset: not enough arguments\n");
		set_last_exit_status(1);
		return ;
	}
	while (cmd->argv[i])
	{
		identifier = cmd->argv[i];
		if (!is_valid_unset_identifier(identifier))
		{
			printf("bash: unset: `%s': not a valid identifier\n",
				identifier);
			has_error = 1;
		}
		else
		{
			unset_env_value(env_list, cmd->argv[i]);
			unset_export_value(export_list, cmd->argv[i]);
		}
		i++;
	}
	set_last_exit_status(has_error ? 1 : 0);
}