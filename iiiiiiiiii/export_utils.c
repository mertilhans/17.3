#include "shell.h"

t_export	*init_export_from_env(t_env *env_list)
{
	t_export	*export_list;
	t_env		*current;

	export_list = NULL;
	current = env_list;
	while (current)
	{
		set_export_value(&export_list, current->key, current->value);
		current = current->next;
	}
	return (export_list);
}