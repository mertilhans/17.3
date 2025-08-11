#include "shell.h"

static t_export	*g_export_list = NULL;

t_export	**get_export_list(void)
{
	return (&g_export_list);
}

t_export	*find_export(t_export *export_list, const char *key)
{
	t_export	*current;

	current = export_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

char	*find_export_value(char *key)
{
	t_export	**export_list;
	t_export	*current;

	export_list = get_export_list();
	current = *export_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0 && current->value)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

static void	set_export_value_existing(t_export *existing, const char *value)
{
	if (existing->value)
		gc_free(existing->value);
	if (value)
		existing->value = ft_strdup((char *)value);
	else
		existing->value = NULL;
}

void	set_export_value(t_export **export_list, const char *key,
					const char *value)
{
	t_export	*existing;
	t_export	*new_export;

	existing = find_export(*export_list, key);
	if (existing)
	{
		set_export_value_existing(existing, value);
		return ;
	}
	new_export = env_gb_malloc(sizeof(t_export));
	if (!new_export)
		return ;
	new_export->key = ft_strdup((char *)key);
	if (value)
		new_export->value = ft_strdup((char *)value);
	else
		new_export->value = NULL;
	new_export->next = *export_list;
	*export_list = new_export;
}