#include "shell.h"

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

	count = 0;
	current = env_list;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
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
			env_array[i] = gb_malloc(ft_strlen(current->key)
					+ ft_strlen(current->value) + 2);
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

void	builtin_pwd(void)
{
	char	*cwd;
	size_t	len;
	ssize_t	write_result;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		len = strlen(cwd);
		write_result = write(STDOUT_FILENO, cwd, len);
		if (write_result == -1)
		{
			perror("write");
			set_last_exit_status(1);
			free(cwd);
			return ;
		}
		write_result = write(STDOUT_FILENO, "\n", 1);
		if (write_result == -1)
		{
			perror("write");
			set_last_exit_status(1);
		}
		else
			set_last_exit_status(0);
		free(cwd);
	}
	else
	{
		perror("pwd");
		set_last_exit_status(1);
	}
}

void	builtin_unset(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	int			has_error;
	char		*identifier;

	i = 1;
	export_list = get_export_list();
	has_error = 0;
	if (!cmd->argv[1])
	{
		printf("unset: not enough arguments\n");
		set_last_exit_status(1);
		return ;
	}
	while (cmd->argv[i])
	{
		identifier = cmd->argv[i];
		if (identifier[0] == '$'
			|| !((identifier[0] >= 'a' && identifier[0] <= 'z')
				|| (identifier[0] >= 'A' && identifier[0] <= 'Z')
				|| identifier[0] == '_'))
		{
			printf("bash: unset: `%s': not a valid identifier\n", identifier);
			has_error = 1;
		}
		else
		{
			unset_env_value(env_list, cmd->argv[i]);
			unset_export_value(export_list, cmd->argv[i]);
		}
		i++;
	}
	set_last_exit_status(has_error);
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