/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 04:24:43 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/25 17:55:38 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "../gb/gb.h"

void *ft_memcpy(void *dst, const void *src, size_t n)
{
    size_t i;
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    if (!dst && !src)
        return NULL;

    i = 0;
    while (i < n)
    {
        d[i] = s[i];
        i++;
    }

    return dst;
}
int is_quote(char c)
{
    return (c == '\'' || c == '"');
}
size_t ft_strlen(char *s)
{
    int i = 0;
    while(s[i] && s[i++])
        ;
    return(i);
}


char *ft_strdup(char *str)
{
    char *res;
    char *result;

    res = gb_malloc(strlen(str) + 1);
    result = res;
    
    while(*str)
        *res++ = *str++;
    *res = '\0';
 return (result);   
}
//