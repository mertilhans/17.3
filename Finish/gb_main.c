/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/17 17:28:19 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdint.h>


void	*gb_malloc(size_t size)
{
	return (gc_safe(malloc(size)));
}

void	gb_free_all(void)
{
	t_gb	**lst;
	t_gb	*tmp;

	lst = get_gb_list();
	if (!lst)
		return ;
	while (*lst)
	{
		tmp = (*lst)->next;
		if ((*lst)->data)
			free((*lst)->data);
		free(*lst);
		*lst = tmp;
	}
	*lst = NULL;
}

void	gc_free_from_list(t_gb **gb_list, void *ptr)
{
	t_gb	*curr;
	t_gb	*prev;

	curr = *gb_list;
	prev = NULL;
	while (curr)
	{
		if ((uintptr_t)curr->data == (uintptr_t)ptr)
		{
			if (prev)
				prev->next = curr->next;
			else
				*gb_list = curr->next;
			free(curr->data);
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

void	gc_free(void *ptr)
{
	t_gb	**gb_list;

	if (!ptr)
		return ;
	gb_list = get_gb_list();
	if (!*gb_list)
	{
		free(ptr);
		return ;
	}
	gc_free_from_list(gb_list, ptr);
}
