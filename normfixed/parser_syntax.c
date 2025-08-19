/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 17:58:17 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/18 18:07:27 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int	ft_syn(t_token *tokens)
{
	t_token	*token;

	token = tokens;
	while (token)
	{
		if (token->type == TOKEN_REDIR_OUT
			|| token->type == TOKEN_REDIR_APPEND
			|| token->type == TOKEN_REDIR_IN
			|| token->type == TOKEN_HEREDOC)
		{
			if (!token->next)
			{
				ft_putendl_fd("bash: syntax error near unexpect"
					"ed token `newline'\n", 2);
				set_last_exit_status(2);
				return (1);
			}
		}
		token = token->next;
	}
	return (0);
}
