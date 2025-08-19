/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:38 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:48:39 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	process_and_validate_args(t_parser *cmd, t_env **env_list)
{
	int			i;
	int			has_error;
	t_export	**export_list;

	i = 1;
	has_error = 0;
	export_list = get_export_list();
	while (cmd->argv[i])
	{
		if (!is_valid_identifier(cmd->argv[i]))
		{
			printf("bash: unset: `%s': not a valid identifier\n", cmd->argv[i]);
			has_error = 1;
		}
		else
		{
			unset_env_value(env_list, cmd->argv[i]);
			unset_export_value(export_list, cmd->argv[i]);
		}
		i++;
	}
	return (has_error);
}

void	builtin_unset(t_parser *cmd, t_env **env_list)
{
	int	has_error;

	if (!cmd->argv[1])
	{
		printf("unset: not enough arguments\n");
		set_last_exit_status(1);
		return ;
	}
	has_error = process_and_validate_args(cmd, env_list);
	if (has_error)
		set_last_exit_status(1);
	else
		set_last_exit_status(0);
}
