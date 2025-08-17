#include "shell.h"

int	only_spaces_before(char *start, char *pos)
{
	while (start < pos)
	{
		if (!ft_ispace(*start))
			return (0);
		start++;
	}
	return (1);
}

int	ft_tokenize_control(char *input)
{
	char	*current;
	char	*temp;

	current = input;
	while (*current)
	{
		if (*current == '|')
		{
			if (only_spaces_before(input, current))
			{
				printf("bash: syntax error near unexpected token `|'\n");
				set_last_exit_status(2);
				return (1);
			}
			temp = current + 1;
			while (*temp && ft_ispace(*temp))
				temp++;
			if (*temp == '\0')
			{
				printf("bash: syntax error near unexpected token `|'\n");
				set_last_exit_status(2);
				return (1);
			}
		}
		current++;
	}
	return (0);
}