/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 18:40:23 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/19 12:25:58 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

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
	if (!s)
		return (NULL);
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

	if (!dest || !src)
		return (dest);
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
