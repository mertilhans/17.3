/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:48:33 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 15:18:38 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdio.h>

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

static void	sort_export_list(t_export **export_list)
{
	t_export	*current;
	int			swapped;

	if (!export_list || !*export_list)
		return ;
	swapped = 1;
	while (swapped)
	{
		swapped = 0;
		current = *export_list;
		while (current && current->next)
		{
			if (ft_strcmp(current->key, current->next->key) > 0)
			{
				swap_export_nodes(current, current->next);
				swapped = 1;
			}
			current = current->next;
		}
	}
}

static void	print_export_list(t_export **export_list)
{
	t_export	*current;

	current = *export_list;
	while (current)
	{
		printf("declare -x %s", current->key);
		if (current->value)
			printf("=\"%s\"", current->value);
		printf("\n");
		current = current->next;
	}
}

void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int	has_error;

	if (!cmd->argv[1])
	{
		sort_export_list(get_export_list());
		print_export_list(get_export_list());
		set_last_exit_status(0);
		return ;
	}
	has_error = export_loop_processor(cmd, env_list);
	if (has_error)
		set_last_exit_status(1);
	else
		set_last_exit_status(0);
}
