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

static void	handle_min_int(char *str, int len)
{
	char	*min;
	int		i;

	min = "-2147483648";
	i = 0;
	while (i < len && min[i])
	{
		str[i] = min[i];
		i++;
	}
	str[i] = '\0';
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	int		is_negative;

	len = get_num_len(n);
	str = gb_malloc(len + 1);
	if (!str)
		return (NULL);
	str[len] = '\0';
	is_negative = 0;
	if (n == 0)
	{
		str[0] = '0';
		return (str);
	}
	if (n < 0)
	{
		is_negative = 1;
		if (n == -2147483648)
		{
			handle_min_int(str, len + 1);
			return (str);
		}
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

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	src_len;

	src_len = 0;
	while (src[src_len])
		src_len++;
	if (size == 0)
		return (src_len);
	i = 0;
	while (i < size - 1 && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (src_len);
}