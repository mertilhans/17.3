#include "shell.h"

static int	get_num_len(int n)
{
	int	len;

	len = 0;
	if (n == 0)
		return (1);
	if (n < 0)
		len = 1;
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

static char	*handle_special_cases(int n, char *str, int len)
{
	if (n == 0)
	{
		str[0] = '0';
		return (str);
	}
	if (n == -2147483648)
	{
		ft_strcpy(str, "-2147483648");
		return (str);
	}
	(void)len;
	return (NULL);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	int		is_negative;
	char	*special;

	len = get_num_len(n);
	str = gb_malloc(len + 1);
	if (!str)
		return (NULL);
	str[len] = '\0';
	special = handle_special_cases(n, str, len);
	if (special)
		return (special);
	is_negative = 0;
	if (n < 0)
	{
		is_negative = 1;
		n = -n;
	}
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	if (is_negative)
		str[0] = '-';
	return (str);
}