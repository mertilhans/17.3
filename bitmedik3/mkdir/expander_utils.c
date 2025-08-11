#include "shell.h"

char	*push_char_res(char *res, char c, int *res_len, int *res_cap)
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

char	*append_string_res(char *result, char *str, int *res_len, int *res_cap)
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