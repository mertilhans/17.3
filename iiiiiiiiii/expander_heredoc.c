#include "shell.h"

static char	*push_heredoc_char(char *res, char c, int *res_len, int *res_cap)
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

static char	*append_heredoc_string(char *result, char *str, int *res_len,
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

__attribute__((unused)) static char	*process_heredoc_char(char *str, int *i, char *result,
	int *res_len, int *res_cap)
{
	char	quote_char;
	char	*expanded;

	quote_char = 0;
	if (str[*i] == '\'' || str[*i] == '\"')
	{
		result = push_heredoc_char(result, str[*i], res_len, res_cap);
		if (quote_char == 0)
			quote_char = str[*i];
		else if (quote_char == str[*i])
			quote_char = 0;
		(*i)++;
	}
	else if (str[*i] == '$')
	{
		expanded = handle_dollar(str, i, NULL);
		if (expanded)
		{
			result = append_heredoc_string(result, expanded, res_len,
				res_cap);
			gc_free(expanded);
		}
	}
	else
	{
		result = push_heredoc_char(result, str[*i], res_len, res_cap);
		(*i)++;
	}
	return (result);
}

char	*expand_heredoc_line(char *str, t_env *env_list)
{
	int		res_len;
	int		res_cap;
	int		i;
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
	while (str[i])
	{
		if (str[i] == '$')
		{
			char *expanded = handle_dollar(str, &i, env_list);
			if (expanded)
			{
				result = append_heredoc_string(result, expanded, &res_len,
					&res_cap);
				gc_free(expanded);
			}
		}
		else
		{
			result = push_heredoc_char(result, str[i], &res_len, &res_cap);
			i++;
		}
	}
	return (result);
}
