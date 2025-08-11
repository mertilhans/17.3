#include "shell.h"

static void	export_swap(t_export *a, t_export *b)
{
	char	*temp_key;
	char	*temp_value;

	temp_key = a->key;
	temp_value = a->value;
	a->key = b->key;
	a->value = b->value;
	b->key = temp_key;
	b->value = temp_value;
}

void	export_sort_list(t_export **export_list)
{
	t_export	*current;
	t_export	*next;
	int			swapped;

	if (!export_list || !*export_list)
		return ;
	swapped = 1;
	while (swapped)
	{
		swapped = 0;
		current = *export_list;
		while (current && current->next)
		{
			next = current->next;
			if (ft_strcmp(current->key, next->key) > 0)
			{
				export_swap(current, next);
				swapped = 1;
			}
			current = current->next;
		}
	}
}

int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!isalpha(str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str[i])
	{
		if (!isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

char	*get_export_key(char *arg)
{
	char	*eq_pos;
	char	*key;
	int		len;

	eq_pos = ft_strchr(arg, '=');
	if (!eq_pos)
		return (ft_strdup(arg));
	len = eq_pos - arg;
	key = gb_malloc(len + 1);
	if (!key)
		return (NULL);
	ft_memcpy(key, arg, len);
	key[len] = '\0';
	return (key);
}

char	*get_export_value(char *arg)
{
	char	*eq_pos;

	eq_pos = ft_strchr(arg, '=');
	if (!eq_pos)
		return (NULL);
	return (ft_strdup(eq_pos + 1));
}
