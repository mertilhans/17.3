#include "shell.h"

int	is_valid_char(char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9') || c == '_');
}

static char	*expand_loop(char *str, char *res, int *res_len,
					int *res_cap, t_env *env_list)
{
	int		i;
	char	*expanded;
	char	*new_res;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			expanded = handle_dollar(str, &i, env_list);
			if (expanded)
			{
				if (*res_len + ft_strlen(expanded) >= *res_cap)
				{
					*res_cap = *res_len + ft_strlen(expanded) + 1;
					new_res = gb_malloc(*res_cap);
					if (!new_res)
						return (NULL);
					ft_memcpy(new_res, res, *res_len);
					gc_free(res);
					res = new_res;
				}
				ft_memcpy(res + *res_len, expanded, ft_strlen(expanded));
				*res_len += ft_strlen(expanded);
				res[*res_len] = '\0';
				gc_free(expanded);
			}
		}
		else
			res = push_char_res(res, str[i++], res_len, res_cap);
	}
	return (res);
}

char	*expand_variables(char *str, t_env *env_list)
{
	char	*res;
	int		res_len;
	int		res_cap;
	int		str_len;

	if (!str)
		return (NULL);
	str_len = ft_strlen(str);
	res_cap = str_len + 1;
	res = gb_malloc(res_cap);
	if (!res)
		return (NULL);
	res[0] = '\0';
	res_len = 0;
	return (expand_loop(str, res, &res_len, &res_cap, env_list));
}

char	*expand_with_quotes(char *str, t_env *env_list)
{
	char	*result;
	char	*expanded;
	char	quote_char;
	int		res_len;
	int		res_cap;
	int		i;

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
		if (str[i] == '\'' || str[i] == '"')
		{
			if (quote_char == 0)
				quote_char = str[i];
			else if (quote_char == str[i])
				quote_char = 0;
			i++;
		}
		else if (str[i] == '$' && quote_char != '\'')
		{
			expanded = handle_dollar(str, &i, env_list);
			if (expanded)
			{
				result = append_string_res(result, expanded,
					&res_len, &res_cap);
				gc_free(expanded);
			}
		}
		else
			result = push_char_res(result, str[i++], &res_len, &res_cap);
	}
	return (result);
}

char	*expand_heredoc_line(char *str, t_env *env_list)
{
	char	*result;
	char	*expanded;
	int		res_len;
	int		res_cap;
	int		i;

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
		if (str[i] == '\'' || str[i] == '\"')
			result = push_char_res(result, str[i++], &res_len, &res_cap);
		else if (str[i] == '$')
		{
			expanded = handle_dollar(str, &i, env_list);
			if (expanded)
			{
				result = append_string_res(result, expanded,
					&res_len, &res_cap);
				gc_free(expanded);
			}
		}
		else
			result = push_char_res(result, str[i++], &res_len, &res_cap);
	}
	return (result);
}