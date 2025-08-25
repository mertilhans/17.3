/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 05:50:11 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/21 18:01:08 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	assign_value(char *key, char *value, t_env **env_list)
{
	t_export	**export_list;

	export_list = get_export_list();
	set_export_value(export_list, key, value);
	set_env_value(env_list, key, value);
}

void	export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
{
	char	*current_arg;
	char	*eq_pos;
	char	*key;
	char	*value;

	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	*eq_pos = '\0';
	key = current_arg;
	value = eq_pos + 1;
	if (ft_strlen(value) > 0)
		assign_value(key, value, env_list);
	else
		assign_value(key, "", env_list);
	*eq_pos = '=';
}
