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