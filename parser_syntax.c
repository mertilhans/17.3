/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 17:58:17 by merilhan          #+#    #+#             */
/*   Updated: 2025/08/19 12:25:21 by husarpka         ###   ########.fr       */
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
