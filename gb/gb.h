# ifndef GB_H
# define GB_H

#include <stdlib.h>

typedef struct s_gb
{
	void		*data;
	struct s_gb	*next;
}	t_gb;

t_gb	*ft_lstnew(void *data);
void	gb_lstadd_front(t_gb **lst, t_gb *new_node);
void *gc_safe(void *ptr);
void *gb_malloc(size_t size);
void	gb_free_all(t_gb **lst);
void    gc_free(void *ptr);
#endif
