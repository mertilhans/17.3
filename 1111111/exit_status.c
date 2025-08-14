/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 04:02:34 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/11 04:02:35 by merilhan         ###   ########.fr       */
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
	int *status_ptr = get_exit_status_adress();
	*status_ptr = status;
}