/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:24:21 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/21 18:15:35 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>

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
		if (curr->data == ptr)
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
