/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 04:24:43 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/10 23:42:38 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	while (len--)
		*s1++ = *s2++;
	*s1 = '\0';
}
int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}
int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s[i] && s[i++])
		;
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

	res = gb_malloc(ft_strlen(str) + 1);
	result = res;
	while (*str)
		*res++ = *str++;
	*res = '\0';
	return (result);
}

char	*ft_strndup(const char *str, size_t n)
{
	char	*result;
	char	*res;
	size_t	i;

	i = 0;
	if (!str)
		return (NULL);
	result = gb_malloc(n + 1);
	if (!result)
		return (NULL);
	res = result;
	while (i < n && str[i])
	{
		*res++ = str[i];
		i++;
	}
	*res = '\0';
	return (result);
}

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
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
			ft_strlcpy(str, "-2147483648", len + 1);
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
