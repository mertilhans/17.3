#include "shell.h"

int	ft_atoi(char *str)
{
	int	sign;
	int	result;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

void	ft_memcpy(char *s1, const char *s2, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		s1[i] = s2[i];
		i++;
	}
}

int	ft_strlen(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] == s2[i])
			i++;
		else
			return (s1[i] - s2[i]);
	}
	return (s1[i] - s2[i]);
}

char	*ft_strdup(char *str)
{
	char	*res;
	char	*result;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	res = gb_malloc(len + 1);
	if (!res)
		return (NULL);
	result = res;
	while (*str)
		*res++ = *str++;
	*res = '\0';
	return (result);
}