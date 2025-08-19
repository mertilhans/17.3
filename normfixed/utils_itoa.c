/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_itoa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 18:41:01 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 21:18:05 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	digit_count(int n)
{
	int	len;

	len = 0;
	if (n < 0)
	{
		len++;
		n = -n;
	}
	while (n != 0)
	{
		n = n / 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char		*str;
	int			len;
	long int	num;

	num = n;
	len = digit_count(num);
	if (n == 0)
		return (ft_strdup("0"));
	str = (char *)gb_malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	str[len] = '\0';
	while (num > 0)
	{
		str[--len] = (num % 10) + '0';
		num = num / 10;
	}
	return (str);
}
void	next_cmd_and_fd(t_exec_data *data)
{
	if (dup2(data->pipefd[1], STDOUT_FILENO) == -1)
	{
		perror("dup2 pipefd[1]");
		exit(1);
	}
	close(data->pipefd[0]);
	close(data->pipefd[1]);
}