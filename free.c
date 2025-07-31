/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: husarpka <husarpka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 04:26:13 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/05 13:39:19 by husarpka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void token_free(t_token *token)
{
    if (!token)
        return;
    
    if (token->value)
        free(token->value);
    free(token);
}



void token_list_free(t_token *tokens)
{
    t_token *current = tokens;
    t_token *next;
    while (current)
    {
        next = current->next;
        token_free(current);
        current = next;
    }

}