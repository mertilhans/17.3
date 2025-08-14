#include "shell.h"

static int	count_env_entries(t_env *env_list)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env_list;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
	return (count);
}

static void	env_bubble_sort(char **env_array, int count)
{
	int		i;
	int		j;
	char	*temp;
	int		swapped;

	if (!env_array || count <= 1)
		return ;
	i = 0;
	while (i < count - 1)
	{
		swapped = 0;
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(env_array[j], env_array[j + 1]) > 0)
			{
				temp = env_array[j];
				env_array[j] = env_array[j + 1];
				env_array[j + 1] = temp;
				swapped = 1;
			}
			j++;
		}
		if (swapped == 0)
			break ;
		i++;
	}
}

static char	**create_sorted_env_array(t_env *env_list)
{
	int		count;
	t_env	*current;
	char	**env_array;
	int		i;
	int		len;

	count = count_env_entries(env_list);
	if (count == 0)
		return (NULL);
	env_array = gb_malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	current = env_list;
	i = 0;
	while (current && i < count)
	{
		if (current->value)
		{
			len = ft_strlen(current->key) + ft_strlen(current->value) + 2;
			env_array[i] = gb_malloc(len);
			if (!env_array[i])
				return (NULL);
			ft_strcpy(env_array[i], current->key);
			ft_strcat(env_array[i], "=");
			ft_strcat(env_array[i], current->value);
			i++;
		}
		current = current->next;
	}
	env_array[i] = NULL;
	env_bubble_sort(env_array, count);
	return (env_array);
}

void	builtin_env(t_env *env_list)
{
	char	**sorted_env;
	int		i;

	sorted_env = create_sorted_env_array(env_list);
	if (!sorted_env)
	{
		set_last_exit_status(0);
		return ;
	}
	i = 0;
	while (sorted_env[i])
	{
		printf("%s\n", sorted_env[i]);
		i++;
	}
	i = 0;
	while (sorted_env[i])
	{
		gc_free(sorted_env[i]);
		i++;
	}
	gc_free(sorted_env);
	set_last_exit_status(0);
}