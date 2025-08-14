/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb_basic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/01 02:17:36 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdint.h>

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
	write(2, "fatal\n", 6);
	exit(1);
}

void	*gc_safe(void *ptr)
{
	t_gb	*new_node;
	t_gb	**gb_list;

	gb_list = get_gb_list();
	if (!ptr)
	{
		fail_exit();
		return (NULL);
	}
	new_node = ft_lstnew(ptr);
	if (!new_node)
	{
		free(ptr);
		fail_exit();
		return (NULL);
	}
	gb_lstadd_front(gb_list, new_node);
	return (ptr);
}
