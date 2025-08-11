#include "shell.h"

static int	check_redirection_end(char *current)
{
	while (*current && ft_ispace(*current))
		current++;
	if (*current == '\0' || *current == '|'
		|| *current == '<' || *current == '>')
	{
		printf("bash: syntax error near unexpected token `newline'\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}

int	check_redirection_syntax(char *input)
{
	char	*current;

	current = input;
	while (*current)
	{
		if (*current == '<' || *current == '>')
		{
			if (*current == '<' && *(current + 1) == '<')
				current += 2;
			else if (*current == '>' && *(current + 1) == '>')
				current += 2;
			else
				current++;
			if (check_redirection_end(current))
				return (1);
		}
		current++;
	}
	return (0);
}

static int	only_spaces_before(char *start, char *pos)
{
	while (start < pos)
	{
		if (!ft_ispace(*start))
			return (0);
		start++;
	}
	return (1);
}

static int	check_pipe_syntax(char *input, char *current)
{
	char	*temp;

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
	return (0);
}

int	ft_tokenize_control(char *input)
{
	char	*current;

	current = input;
	while (*current)
	{
		if (*current == '|')
		{
			if (check_pipe_syntax(input, current))
				return (1);
		}
		current++;
	}
	return (0);
}