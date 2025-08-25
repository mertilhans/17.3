/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 04:02:34 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/21 17:58:50 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <sys/wait.h>

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
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			return (130);
		else if (sig == SIGQUIT)
			return (131);
		else
			return (128 + sig);
	}
	return (1);
}
