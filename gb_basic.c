/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb_basic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:24:10 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/22 23:39:34 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>

t_gb	**get_gb_list(void)
{
	static t_gb	*list = NULL;

	return (&list);
}

t_gb	*ft_lstnew(void *data)
{
	t_gb	*node;

	node = (t_gb *)malloc(sizeof(t_gb));
	if (!node)
		return (NULL);
	node->data = data;
	node->next = NULL;
	return (node);
}

void	gb_lstadd_front(t_gb **lst, t_gb *new_node)
{
	if (!lst || !new_node)
		return ;
	new_node->next = *lst;
	*lst = new_node;
}

void	fail_exit(void)
{
	write(2, "fatal error\n", 12);
	gb_free_all();
	env_gb_free_all();
	exit(1);
}

void	*gc_safe(void *ptr)
{
	t_gb	*new_node;
	t_gb	**gb_list;

	gb_list = get_gb_list();
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
