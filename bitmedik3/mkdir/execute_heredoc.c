#include "shell.h"

static int	handle_heredoc_interrupt(t_heredoc_buffer *buf)
{
	gc_free(buf->content);
	setup_interactive_signals();
	return (-1);
}

static char	*handle_eof_heredoc(t_heredoc_buffer *buf, const char *delimiter)
{
	printf("bash: warning: here-document at line 1 ");
	printf("delimited by end-of-file (wanted `%s')\n", delimiter);
	set_last_exit_status(0);
	return (buf->content);
}

char	*readline_loop(t_heredoc_buffer *buf, const char *delimiter)
{
	heredoc_signals();
	while (1)
	{
		errno = 0;
		buf->line = heredoc_readline("> ");
		if (!buf->line)
		{
			if (errno == EINTR)
				return ((char *)(long)handle_heredoc_interrupt(buf));
			else
				return (handle_eof_heredoc(buf, delimiter));
		}
		if ((size_t)ft_strlen(buf->line) == buf->delimiter_len
			&& ft_strcmp(buf->line, delimiter) == 0)
		{
			gc_free(buf->line);
			break ;
		}
		if (!heredoc_append_line(buf))
			return (NULL);
		gc_free(buf->line);
		buf->line = NULL;
	}
	setup_interactive_signals();
	return (buf->content);
}

static char	*init_heredoc_buffer(t_heredoc_buffer *buf, const char *delimiter)
{
	buf->content = ft_strdup("");
	buf->line = NULL;
	buf->line_with_nl = NULL;
	buf->new_content = NULL;
	buf->delimiter_len = ft_strlen(delimiter);
	if (!buf->content)
	{
		perror("Heredoc malloc error");
		return (NULL);
	}
	return (buf->content);
}

char	*read_single_heredoc_block(char *delimiter)
{
	t_heredoc_buffer	buf;
	char				*result;

	if (!delimiter || *delimiter == '\0')
	{
		printf("Heredoc delimiter error\n");
		return (NULL);
	}
	if (!init_heredoc_buffer(&buf, delimiter))
		return (NULL);
	result = readline_loop(&buf, delimiter);
	if (!result && get_last_exit_status() == 130)
	{
		setup_interactive_signals();
		return (NULL);
	}
	setup_interactive_signals();
	return (result);
}
