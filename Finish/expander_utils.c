/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 02:46:32 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/17 04:37:34 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	is_valid_char(char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0'
			&& c <= '9') || c == '_');
}

static void	copy_word(char **result, char *str, int start, int end)
{
	int	len;

	len = end - start;
	*result = gb_malloc(len + 1);
	if (!*result)
		return ;
	ft_memcpy(*result, str + start, len);
	(*result)[len] = '\0';
}

static void	split_words_loop(char *str, char **result, int *count)
{
	int	i;
	int	start;

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
		copy_word(&result[*count], str, start, i);
		if (!result[*count])
			return ;
		(*count)++;
	}
}

char	**split_expanded_string(char *str)
{
	char	**result;
	int		count;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = gb_malloc(sizeof(char *) * (len / 2 + 10));
	if (!result)
		return (NULL);
	count = 0;
	split_words_loop(str, result, &count);
	result[count] = NULL;
	return (result);
}

char	*realloc_result(char *res, int total_len)
{
	char *new_res;

	new_res = gb_malloc(total_len + 1);
	if (!new_res)
	{
		gc_free(res);
		return (NULL);
	}
	ft_memcpy(new_res, res, ft_strlen(res));
	gc_free(res);
	return (new_res);
}