/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 04:02:34 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:49:18 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	*get_exit_status_adress(void)
{
	static int	last_exit_status = 0;

	return (&last_exit_status);
}
int	get_last_exit_status(void)
{
	int	*status_ptr;

	status_ptr = get_exit_status_adress();
	return (*status_ptr);
}
void	set_last_exit_status(int status)
{
	int	*status_ptr;

	status_ptr = get_exit_status_adress();
	*status_ptr = status;
}
int	calculate_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		if (sig == SIGINT)
			return (130);
		else if (sig == SIGQUIT)
			return (131);
		else
			return (128 + sig);
	}
	return (1);
}