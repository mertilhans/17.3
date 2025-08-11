#include "shell.h"

static char	*handle_exit_status(int *i)
{
	(*i)++;
	return (ft_itoa(get_last_exit_status()));
}

static char	*handle_pid(int *i)
{
	(*i)++;
	return (ft_itoa(getpid()));
}

static char	*extract_var_name(char *str, int *i)
{
	int		start_i;
	int		len;
	char	*var_name;

	start_i = *i;
	while (str[*i] && (isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	len = *i - start_i;
	if (len <= 0)
		return (NULL);
	var_name = gb_malloc(len + 1);
	if (!var_name)
		return (NULL);
	ft_memcpy(var_name, str + start_i, len);
	var_name[len] = '\0';
	return (var_name);
}

static char	*get_var_value(char *var_name, t_env *env_list)
{
	t_env	*env_node;
	char	*export_value;
	char	*result;

	env_node = find_env(env_list, var_name);
	if (env_node && env_node->value)
	{
		result = ft_strdup(env_node->value);
		gc_free(var_name);
		return (result);
	}
	export_value = find_export_value(var_name);
	if (export_value)
	{
		result = ft_strdup(export_value);
		gc_free(var_name);
		return (result);
	}
	gc_free(var_name);
	return (ft_strdup(""));
}

char	*handle_dollar(char *str, int *i, t_env *env_list)
{
	char	*var_name;

	(*i)++;
	if (!str[*i])
		return (ft_strdup("$"));
	if (str[*i] == '?')
		return (handle_exit_status(i));
	if (str[*i] == '$')
		return (handle_pid(i));
	if (isalpha(str[*i]) || str[*i] == '_')
	{
		var_name = extract_var_name(str, i);
		if (!var_name)
			return (ft_strdup(""));
		return (get_var_value(var_name, env_list));
	}
	return (ft_strdup("$"));
}