#include "shell.h"

static char	*push_char_res(char *res, char c, int *res_len, int *res_cap)
{
	char	*new_res;

	if (*res_len + 1 >= *res_cap)
	{
		*res_cap = *res_len + 2;
		new_res = gb_malloc(*res_cap);
		if (!new_res)
			return (res);
		if (res)
		{
			ft_memcpy(new_res, res, *res_len);
			gc_free(res);
		}
		else
			new_res[0] = '\0';
		res = new_res;
	}
	res[*res_len] = c;
	(*res_len)++;
	res[*res_len] = '\0';
	return (res);
}

static char	*append_string_res(char *result, char *str, int *res_len,
	int *res_cap)
{
	int		add_len;
	char	*new_res;

	add_len = ft_strlen(str);
	if (*res_len + add_len >= *res_cap)
	{
		*res_cap = *res_len + add_len + 1;
		new_res = gb_malloc(*res_cap);
		if (!new_res)
			return (result);
		if (result)
		{
			ft_memcpy(new_res, result, *res_len);
			gc_free(result);
		}
		else
			new_res[0] = '\0';
		result = new_res;
	}
	ft_memcpy(result + *res_len, str, add_len);
	*res_len += add_len;
	result[*res_len] = '\0';
	return (result);
}

static void	handle_quotes(char *str, int *i, char *quote_char)
{
	if (str[*i] == '\'' || str[*i] == '"')
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
		else
			(*i)++;
	}
}

static char	*process_quote_or_dollar(char *str, int *i, char *result,
	t_env *env_list, char *quote_char, int *res_len, int *res_cap)
{
	char	*expanded;

	if (str[*i] == '\'' || str[*i] == '"')
	{
		handle_quotes(str, i, quote_char);
		if (*i > 0 && str[*i - 1] == *quote_char)
			return (result);
		result = push_char_res(result, str[*i - 1], res_len, res_cap);
	}
	else if (str[*i] == '$' && *quote_char != '\'')
	{
		expanded = handle_dollar(str, i, env_list);
		if (expanded)
		{
			result = append_string_res(result, expanded, res_len, res_cap);
			gc_free(expanded);
		}
	}
	else
	{
		result = push_char_res(result, str[*i], res_len, res_cap);
		(*i)++;
	}
	return (result);
}

char	*expand_with_quotes(char *str, t_env *env_list)
{
	int		res_len;
	int		res_cap;
	int		i;
	char	quote_char;
	char	*result;

	if (!str)
		return (NULL);
	res_cap = ft_strlen(str) + 1;
	result = gb_malloc(res_cap);
	if (!result)
		return (NULL);
	result[0] = '\0';
	res_len = 0;
	i = 0;
	quote_char = 0;
	while (str[i])
	{
		result = process_quote_or_dollar(str, &i, result, env_list,
			&quote_char, &res_len, &res_cap);
	}
	return (result);
}