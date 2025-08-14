#include "shell.h"

static char	*handle_special_vars(char *str, int *i)
{
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(get_last_exit_status()));
	}
	if (str[*i] == '$')
	{
		(*i)++;
		return (ft_itoa(getpid()));
	}
	return (NULL);
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
		return (ft_strdup(""));
	var_name = gb_malloc(len + 1);
	if (!var_name)
		return (ft_strdup(""));
	ft_memcpy(var_name, str + start_i, len);
	var_name[len] = '\0';
	return (var_name);
}

static char	*get_var_value(char *var_name, t_env *env_list)
{
	t_env	*env_node;
	char	*export_value;
	char	*expanded;

	env_node = find_env(env_list, var_name);
	if (env_node && env_node->value)
	{
		expanded = ft_strdup(env_node->value);
		gc_free(var_name);
		return (expanded ? expanded : ft_strdup(""));
	}
	export_value = find_export_value(var_name);
	if (export_value)
	{
		expanded = ft_strdup(export_value);
		gc_free(var_name);
		return (expanded ? expanded : ft_strdup(""));
	}
	gc_free(var_name);
	return (ft_strdup(""));
}

char	*handle_dollar(char *str, int *i, t_env *env_list)
{
	char	*var_name;
	char	*result;

	(*i)++;
	if (!str[*i])
		return (ft_strdup("$"));
	result = handle_special_vars(str, i);
	if (result)
		return (result);
	if (isalpha(str[*i]) || str[*i] == '_')
	{
		var_name = extract_var_name(str, i);
		if (!var_name)
			return (ft_strdup(""));
		return (get_var_value(var_name, env_list));
	}
	return (ft_strdup("$"));
}
