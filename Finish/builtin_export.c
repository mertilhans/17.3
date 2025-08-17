/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:33 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/16 05:48:34 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	process_export_argument(t_parser *cmd, int *i, t_env **env_list)
{
	char		*current_arg;
	char		*eq_pos;
	t_export	**export_list;

	export_list = get_export_list();
	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	if (eq_pos)
		export_process_keyvalue(cmd, i, env_list);
	else
		set_export_value(export_list, cmd->argv[*i], NULL);
}

static int	export_loop_processor(t_parser *cmd, t_env **env_list)
{
	int	i;
	int	has_error;

	i = 1;
	has_error = 0;
	while (cmd->argv[i])
	{
		if (!is_valid_identifier(cmd->argv[i]))
		{
			printf("bash: export: `%s': not a valid identifier\n",
					cmd->argv[i]);
			has_error = 1;
		}
		else
			process_export_argument(cmd, &i, env_list);
		i++;
	}
	return (has_error);
}

void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int has_error;

	if (!cmd->argv[1])
	{
		export_print_all(get_export_list());
		set_last_exit_status(0);
		return ;
	}
	has_error = export_loop_processor(cmd, env_list);
	if (has_error)
		set_last_exit_status(1);
	else
		set_last_exit_status(0);
}