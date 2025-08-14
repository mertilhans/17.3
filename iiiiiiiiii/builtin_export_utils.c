#include "shell.h"

__attribute__((unused)) static char	*build_value_with_args(t_parser *cmd, int *i, char *value)
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
