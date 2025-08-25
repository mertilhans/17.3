/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:24:16 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/22 23:39:57 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>

t_gb	**get_env_gb_list(void)
{
	static t_gb	*env_list = NULL;

	return (&env_list);
}

void	*env_gc_safe(void *ptr)
{
	t_gb	*new_node;
	t_gb	**gb_list;

	gb_list = get_env_gb_list();
	if (!ptr)
		fail_exit();
	new_node = ft_lstnew(ptr);
	if (!new_node)
	{
		free(ptr);
		fail_exit();
	}
	gb_lstadd_front(gb_list, new_node);
	return (ptr);
}

void	*env_gb_malloc(size_t size)
{
	return (env_gc_safe(malloc(size)));
}

void	env_gb_free_all(void)
{
	t_gb	**lst;
	t_gb	*tmp;

	lst = get_env_gb_list();
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

void	env_gc_free(void *ptr)
{
	t_gb	**gb_list;

	if (!ptr)
		return ;
	gb_list = get_env_gb_list();
	if (!*gb_list)
	{
		free(ptr);
		return ;
	}
	gc_free_from_list(gb_list, ptr);
}
