#include "shell.h"

/* Forward declarations */
void	export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list);

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

static int	is_valid_identifier(char *identifier)
{
	if (identifier[0] == '$')
		return (0);
	if (!((identifier[0] >= 'a' && identifier[0] <= 'z')
			|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
			|| identifier[0] == '_'))
		return (0);
	return (1);
}

static void	process_export_arg(t_parser *cmd, int *i, t_env **env_list,
	int *has_error)
{
	char		*current_arg;
	char		*eq_pos;
	char		*identifier;
	t_export	**export_list;

	export_list = get_export_list();
	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	identifier = current_arg;
	if (eq_pos)
	{
		*eq_pos = '\0';
		identifier = current_arg;
	}
	if (!is_valid_identifier(identifier))
	{
		printf("bash: export: `%s': not a valid identifier\n", identifier);
		*has_error = 1;
		if (eq_pos)
			*eq_pos = '=';
	}
	else
	{
		if (eq_pos)
		{
			*eq_pos = '=';
			export_process_keyvalue(cmd, i, env_list);
		}
		else
			set_export_value(export_list, cmd->argv[*i], NULL);
	}
}

void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	int			has_error;

	i = 1;
	export_list = get_export_list();
	has_error = 0;
	if (!cmd->argv[1])
	{
		export_print_all(export_list);
		set_last_exit_status(0);
		return ;
	}
	while (cmd->argv[i])
	{
		process_export_arg(cmd, &i, env_list, &has_error);
		i++;
	}
	set_last_exit_status(has_error ? 1 : 0);
}

/* Export process keyvalue function */
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
	{
		if (cmd->argv[*i + 1])
		{
			char *full_value = export_build_value(cmd, i, value);
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
	else
	{
		set_export_value(export_list, key, "");
		set_env_value(env_list, key, "");
	}
	*eq_pos = '=';
}

/* Export build value function - defined in builtin_export_utils.c */
char	*export_build_value(t_parser *cmd, int *i, char *value)
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