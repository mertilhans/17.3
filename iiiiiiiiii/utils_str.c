#include "shell.h"

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

char	*ft_strcpy(char *dest, const char *src)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_strcat(char *dest, const char *src)
{
	size_t	i;
	size_t	j;

	j = 0;
	i = 0;
	while (dest[i])
		i++;
	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return (dest);
}