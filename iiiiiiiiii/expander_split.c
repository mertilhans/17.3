#include "shell.h"

static int	count_words(char *str)
{
	int	count;
	int	i;
	int	in_word;

	count = 0;
	i = 0;
	in_word = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t')
		{
			if (!in_word)
			{
				count++;
				in_word = 1;
			}
		}
		else
			in_word = 0;
		i++;
	}
	return (count);
}

static char	*extract_word(char *str, int start, int len)
{
	char	*word;

	word = gb_malloc(len + 1);
	if (!word)
		return (NULL);
	ft_memcpy(word, str + start, len);
	word[len] = '\0';
	return (word);
}

char	**split_expanded_string(char *str)
{
	char	**result;
	int		count;
	int		i;
	int		start;
	int		word_idx;

	if (!str)
		return (NULL);
	count = count_words(str);
	result = gb_malloc(sizeof(char *) * (count + 1));
	if (!result)
		return (NULL);
	i = 0;
	word_idx = 0;
	while (str[i])
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t'))
			i++;
		if (!str[i])
			break ;
		start = i;
		while (str[i] && str[i] != ' ' && str[i] != '\t')
			i++;
		result[word_idx] = extract_word(str, start, i - start);
		if (!result[word_idx])
			return (NULL);
		word_idx++;
	}
	result[word_idx] = NULL;
	return (result);
}
