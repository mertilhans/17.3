#include "shell.h"

t_env	*find_env(t_env *env_list, const char *key)
{
	t_env	*current;

	if (!env_list || !key)
		return (NULL);
	current = env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*env_variable;

	env_variable = find_env(env_list, key);
	if (env_variable)
		return (env_variable->value);
	return (NULL);
}