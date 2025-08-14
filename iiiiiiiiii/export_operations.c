#include "shell.h"

void	set_export_value(t_export **export_list, const char *key,
	const char *value)
{
	t_export	*existing;

	existing = find_export(*export_list, key);
	if (existing)
		set_export_value_existing(existing, value);
	else
		set_export_value_new(export_list, key, value);
}

void	unset_export_value(t_export **export_list, const char *key)
{
	t_export	*current;
	t_export	*prev;

	current = *export_list;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*export_list = current->next;
			if (current->key)
				free(current->key);
			if (current->value)
				free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

void	free_export_list(t_export *export_list)
{
	t_export	*current;
	t_export	*next;

	current = export_list;
	while (current)
	{
		next = current->next;
		if (current->key)
			free(current->key);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}