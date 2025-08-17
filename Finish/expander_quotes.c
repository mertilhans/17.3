/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:49:24 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:25 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char	*realloc_char_res(char *res, int res_len)
{
	char	*new_res;

	new_res = gb_malloc(res_len + 2);
	if (!new_res)
		return (res);
	if (res)
	{
		ft_memcpy(new_res, res, res_len);
		gc_free(res);
	}
	else
		new_res[0] = '\0';
	return (new_res);
}

char	*push_char_res(char *res, char c, int *res_len, int *res_cap)
{
	if (*res_len + 1 >= *res_cap)
	{
		*res_cap = *res_len + 2;
		res = realloc_char_res(res, *res_len);
	}
	res[*res_len] = c;
	(*res_len)++;
	res[*res_len] = '\0';
	return (res);
}

char	*realloc_string_res(char *result, int res_len, int add_len)
{
	char	*new_res;

	new_res = gb_malloc(res_len + add_len + 1);
	if (!new_res)
		return (result);
	if (result)
	{
		ft_memcpy(new_res, result, res_len);
		gc_free(result);
	}
	else
		new_res[0] = '\0';
	return (new_res);
}

char	*append_string_res(char *result, char *str, int *res_len, int *res_cap)
{
	int	add_len;

	add_len = ft_strlen(str);
	if (*res_len + add_len >= *res_cap)
	{
		*res_cap = *res_len + add_len + 1;
		result = realloc_string_res(result, *res_len, add_len);
	}
	ft_memcpy(result + *res_len, str, add_len);
	*res_len += add_len;
	result[*res_len] = '\0';
	return (result);
}

void	process_quote_char(char *str, int *i, char *quote_char)
{
	if (*quote_char == 0)
	{
		*quote_char = str[*i];
		(*i)++;
	}
	else if (*quote_char == str[*i])
	{
		*quote_char = 0;
		(*i)++;
	}
}