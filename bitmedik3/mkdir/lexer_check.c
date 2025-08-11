#include "shell.h"

static int	check_pipe_at_start(char *input)
{
	int	i;

	i = 0;
	while (input[i] && ft_ispace(input[i]))
		i++;
	if (input[i] == '|')
	{
		printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}

static int	check_pipe_at_end(char *input)
{
	int	i;

	i = ft_strlen(input) - 1;
	while (i >= 0 && ft_ispace(input[i]))
		i--;
	if (i >= 0 && input[i] == '|')
	{
		printf("bash: syntax error near unexpected token `|'\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}

static int	check_double_pipe(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] == '|')
		{
			i++;
			while (input[i] && ft_ispace(input[i]))
				i++;
			if (input[i] == '|')
			{
				printf("bash: syntax error near unexpected token `|'\n");
				set_last_exit_status(2);
				return (1);
			}
		}
		else
			i++;
	}
	return (0);
}

int	check_pipe_errors(char *input)
{
	if (check_pipe_at_start(input))
		return (1);
	if (check_pipe_at_end(input))
		return (1);
	if (check_double_pipe(input))
		return (1);
	return (0);
}

int	check_quote_errors(char *input)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (input[i])
	{
		if ((input[i] == '"' || input[i] == '\'') && quote == 0)
			quote = input[i];
		else if (input[i] == quote)
			quote = 0;
		i++;
	}
	if (quote != 0)
	{
		printf("bash: syntax error: unterminated quoted string\n");
		set_last_exit_status(2);
		return (1);
	}
	return (0);
}