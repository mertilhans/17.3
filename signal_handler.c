/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:25:43 by husarpka          #+#    #+#             */
/*   Updated: 2025/08/25 19:39:14 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <readline/readline.h>
#include <stdio.h>

volatile sig_atomic_t	g_signal_status = 0;

void	ft_prompt_handler(int signum)
{
	g_signal_status = signum;
	set_last_exit_status(130);
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	ft_sigquit_handler(int signum)
{
	g_signal_status = signum;
	ft_putendl_fd("Quit (core dumped)", 2);
}

void	ft_execute_handler(int signum)
{
	g_signal_status = signum;
	write(STDOUT_FILENO, "\n", 1);
}

void	ft_heredoc_handler(int signum)
{
	g_signal_status = signum;
	rl_done = 1;
	close(STDIN_FILENO);
}

static int	ft_event_hook(void)
{
	return (0);
}

static void	setup_signal_action(int sign, void (*handler)(int))
{
	struct sigaction	sa;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(sign, &sa, NULL);
}

void	ft_init_signals(t_signal_type context_type)
{
	g_signal_status = 0;
	if (context_type == PROMPT)
	{
		rl_event_hook = NULL;
		setup_signal_action(SIGINT, ft_prompt_handler);
		setup_signal_action(SIGQUIT, SIG_IGN);
	}
	else if (context_type == EXECUTION)
	{
		rl_event_hook = NULL;
		setup_signal_action(SIGINT, ft_execute_handler);
		setup_signal_action(SIGQUIT, ft_sigquit_handler);
	}
	else if (context_type == HEREDOC)
	{
		rl_event_hook = ft_event_hook;
		setup_signal_action(SIGINT, ft_heredoc_handler);
		setup_signal_action(SIGQUIT, SIG_IGN);
	}
	else if (context_type == IGNORE)
	{
		setup_signal_action(SIGINT, SIG_IGN);
		setup_signal_action(SIGQUIT, SIG_IGN);
	}
	else if (context_type == DEFAULT)
	{
		setup_signal_action(SIGINT, SIG_DFL);
		setup_signal_action(SIGQUIT, SIG_DFL);
	}
}
