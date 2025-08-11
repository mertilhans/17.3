#include "shell.h"

static void	export_print_all(t_export **export_list)
{
	t_export	*current;

	current = *export_list;
	while (current)
	{
		printf("declare -x %s", current->key);
		if (current->value != NULL)
			printf("=\"%s\"", current->value);
		printf("\n");
		current = current->next;
	}
}

static char	*export_build_value(t_parser *cmd, int *i, char *value)
{
	char	*full_value;
	int		j;
	int		total_len;

	total_len = ft_strlen(value);
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		total_len += ft_strlen(cmd->argv[j]) + 1;
		j++;
	}
	full_value = gb_malloc(total_len + 1);
	ft_strcpy(full_value, value);
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		ft_strcat(full_value, " ");
		ft_strcat(full_value, cmd->argv[j]);
		j++;
	}
	*i = j - 1;
	return (full_value);
}

static void	process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
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
	{
		if (cmd->argv[*i + 1])
		{
			value = export_build_value(cmd, i, value);
			set_export_value(export_list, key, value);
			set_env_value(env_list, key, value);
			free(value);
		}
		else
		{
			set_export_value(export_list, key, value);
			set_env_value(env_list, key, value);
		}
	}
	else
	{
		set_export_value(export_list, key, "");
		set_env_value(env_list, key, "");
	}
	*eq_pos = '=';
}

static int	validate_identifier(char *identifier)
{
	if (identifier[0] == '$')
	{
		printf("bash: export: `%s': not a valid identifier\n", identifier);
		return (0);
	}
	if (!((identifier[0] >= 'a' && identifier[0] <= 'z')
			|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
			|| identifier[0] == '_'))
	{
		printf("bash: export: `%s': not a valid identifier\n", identifier);
		return (0);
	}
	return (1);
}

void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	int			has_error;
	char		*eq_pos;

	i = 1;
	has_error = 0;
	export_list = get_export_list();
	if (!cmd->argv[1])
	{
		export_print_all(export_list);
		set_last_exit_status(0);
		return ;
	}
	while (cmd->argv[i])
	{
		eq_pos = ft_strchr(cmd->argv[i], '=');
		if (eq_pos)
			*eq_pos = '\0';
		if (!validate_identifier(cmd->argv[i]))
			has_error = 1;
		else
		{
			if (eq_pos)
			{
				*eq_pos = '=';
				process_keyvalue(cmd, &i, env_list);
			}
			else
				set_export_value(export_list, cmd->argv[i], NULL);
		}
		if (eq_pos && !validate_identifier(cmd->argv[i]))
			*eq_pos = '=';
		i++;
	}
	set_last_exit_status(has_error);
}
