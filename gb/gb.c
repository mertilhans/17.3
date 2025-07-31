/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gb.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 05:32:23 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/06 22:41:12 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gb.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

static t_gb    **get_gb_list(void)
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
static void fail_exit(void)
{
    write(2, "fatal\n", 6);
    exit(1);
}
void *gc_safe(void *ptr)
{
    t_gb    *new_node;
    t_gb    **gb_list;
    
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

void *gb_malloc(size_t size)
{
    return (gc_safe(malloc(size)));
}
void	gb_free_all(t_gb **lst)
{
	t_gb	*tmp;

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
void    gc_free(void *ptr)
{
    t_gb    **gb_list;
    t_gb    *curr;
    t_gb    *prev;

    if (!ptr)
        return ;
    gb_list = get_gb_list();
    if (!*gb_list)
    {
        free(ptr);
        return ;
    }
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



