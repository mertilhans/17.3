#include "shell.h"

char	*heredoc_readline(const char *prompt)
{
	t_heredoc_state	st;
	ssize_t			bytes_read;
	char			ch;
	int				status;

	if (get_last_exit_status() == 130)
		set_last_exit_status(0);
	if (prompt)
		write(STDOUT_FILENO, prompt, strlen(prompt));
	st.line = NULL;
	st.len = 0;
	st.at_line_start = 1;
	while (1)
	{
		bytes_read = read(STDIN_FILENO, &ch, 1);
		status = process_read_char(bytes_read, ch, &st);
		if (status == 0)
			return (finalize_line(&st));
		if (status == -1)
			return (NULL);
	}
}

int	process_read_char(ssize_t bytes, char ch, t_heredoc_state *st)
{
	if (bytes <= 0)
	{
		if (bytes == 0)
		{
			write(STDOUT_FILENO, "\n", 1);
			
			if (bytes == 0 && !(st->at_line_start && st->len == 0))
				return (1);
		}
		if (st->line)
			gc_free(st->line);
		return (-1);
	}
	if (ch == '\n')
		return (0);
	if (ch == '\r')
		return (1);
	st->at_line_start = 0;
	if (!append_char(st, ch))
		return (-1);
	return (1);
}

int	append_char(t_heredoc_state *st, char ch)
{
	char	*new_line;

	new_line = gb_malloc(st->len + 1);
	if (!new_line)
	{
		if (st->line)
			gc_free(st->line);
		return (0);
	}
	if (st->line)
	{
		ft_memcpy(new_line, st->line, st->len);
		gc_free(st->line);
	}
	new_line[st->len] = ch;
	st->line = new_line;
	st->len++;
	return (1);
}

char	*finalize_line(t_heredoc_state *st)
{
	char	*final_line;

	if (!st->line)
	{
		final_line = gb_malloc(sizeof(char));
		if (final_line)
			final_line[0] = '\0';
		return (final_line);
	}
	final_line = gb_malloc(st->len + 1);
	if (!final_line)
	{
		gc_free(st->line);
		return (NULL);
	}
	ft_memcpy(final_line, st->line, st->len);
	final_line[st->len] = '\0';
	gc_free(st->line);
	return (final_line);
}