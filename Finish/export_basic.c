/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_basic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/14 20:34:47 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>
#include <string.h>

t_export	**get_export_list(void)
{
	static t_export	*g_export_list;

	g_export_list = NULL;
	return (&g_export_list);
}

t_export	*find_export(t_export *export_list, const char *key)
{
	t_export	*current;

	current = export_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

char	*find_export_value(char *key)
{
	t_export	**export_list;
	t_export	*current;

	export_list = get_export_list();
	current = *export_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0 && current->value)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	set_export_value_existing(t_export *existing, const char *value)
{
	if (existing->value)
		gc_free(existing->value);
	if (value)
		existing->value = ft_strdup((char *)value);
	else
		existing->value = NULL;
}

void	set_export_value_new(t_export **export_list, const char *key,
		const char *value)
{
	t_export	*new_export;

	new_export = env_gb_malloc(sizeof(t_export));
	if (!new_export)
		return ;
	new_export->key = ft_strdup((char *)key);
	if (value)
		new_export->value = ft_strdup((char *)value);
	else
		new_export->value = NULL;
	new_export->next = *export_list;
	*export_list = new_export;
}

void	set_export_value(t_export **export_list, const char *key,
		const char *value)
{
	t_export	*existing;

	existing = find_export(*export_list, key);
	if (existing)
		set_export_value_existing(existing, value);
	else
		set_export_value_new(export_list, key, value);
}
