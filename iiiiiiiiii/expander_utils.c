#include "shell.h"

int	is_valid_char(char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9') || c == '_');
}

char	*expand_variables(char *str, t_env *env_list)
{
	char	*res;
	char	*expanded;
	char	*new_res;
	int		res_len;
	int		res_cap;

	if (!str)
		return (NULL);
	res_cap = ft_strlen(str) + 1;
	res = gb_malloc(res_cap);
	if (!res)
		return (NULL);
	res[0] = '\0';
	res_len = 0;
	int i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			expanded = handle_dollar(str, &i, env_list);
			if (expanded)
			{
				int add_len = ft_strlen(expanded);
				if (res_len + add_len >= res_cap)
				{
					res_cap = res_len + add_len + 1;
					new_res = gb_malloc(res_cap);
					if (!new_res)
					{
						gc_free(res);
						gc_free(expanded);
						return (NULL);
					}
					ft_memcpy(new_res, res, res_len);
					gc_free(res);
					res = new_res;
				}
				ft_memcpy(res + res_len, expanded, add_len);
				res_len += add_len;
				res[res_len] = '\0';
				gc_free(expanded);
			}
		}
		else
		{
			if (res_len + 1 >= res_cap)
			{
				res_cap = res_len + 2;
				new_res = gb_malloc(res_cap);
				if (!new_res)
				{
					gc_free(res);
					return (NULL);
				}
				ft_memcpy(new_res, res, res_len);
				gc_free(res);
				res = new_res;
			}
			res[res_len] = str[i];
			res_len++;
			res[res_len] = '\0';
			i++;
		}
	}
	return (res);
}