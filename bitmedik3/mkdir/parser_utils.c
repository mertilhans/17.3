#include "shell.h"

int	has_quote_chars(char *delimiter)
{
	int	i;
	int	len;

	i = 0;
	len = ft_strlen(delimiter);
	while (i < len)
	{
		if (delimiter[i] == '"' || delimiter[i] == '\'')
			return (1);
		i++;
	}
	return (0);
}

char	*remove_quotes(char *delimiter)
{
	char	*clean_delimiter;
	int		i;
	int		j;
	int		len;

	len = ft_strlen(delimiter);
	clean_delimiter = gb_malloc(len + 1);
	if (!clean_delimiter)
		return (NULL);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (delimiter[i] != '"' && delimiter[i] != '\'')
			clean_delimiter[j++] = delimiter[i];
		i++;
	}
	clean_delimiter[j] = '\0';
	return (clean_delimiter);
}

char	**split_expanded_string(char *str)
{
	char	**result;
	int		count;
	int		i;
	int		start;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = gb_malloc(sizeof(char *) * (len / 2 + 10));
	if (!result)
		return (NULL);
	count = 0;
	i = 0;
	while (str[i])
	{
		while (str[i] == ' ' || str[i] == '\t')
			i++;
		if (!str[i])
			break ;
		start = i;
		while (str[i] && str[i] != ' ' && str[i] != '\t')
			i++;
		result[count] = gb_malloc(i - start + 1);
		if (!result[count])
			return (NULL);
		ft_memcpy(result[count], str + start, i - start);
		result[count][i - start] = '\0';
		count++;
	}
	result[count] = NULL;
	return (result);
}