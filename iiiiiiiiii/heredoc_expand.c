#include "shell.h"

static char	*handle_line_end(t_heredoc_buffer *buf, const char *delimiter)
{
	if (errno == EINTR)
	{
		write(STDOUT_FILENO, "", 0);
		set_last_exit_status(130);
		return (NULL);
	}
	else
	{
		printf("bash: warning: here-document delimited by ");
		printf("end-of-file (wanted `%s')\n", delimiter);
		set_last_exit_status(0);
		return (buf->content);
	}
}

char	*readline_loop_expand(t_heredoc_buffer *buf, const char *delimiter,
	t_env *env_list)
{
	heredoc_signals();
	while (1)
	{
		errno = 0;
		buf->line = heredoc_readline("> ");
		if (!buf->line)
			return (handle_line_end(buf, delimiter));
		if ((size_t)ft_strlen(buf->line) == buf->delimiter_len
			&& ft_strcmp(buf->line, delimiter) == 0)
		{
			gc_free(buf->line);
			break ;
		}
		if (!heredoc_append_expanded(buf, env_list))
		{
			perror("line error");
			gc_free(buf->line);
			gc_free(buf->content);
			setup_interactive_signals();
			return (NULL);
		}
		gc_free(buf->line);
		buf->line = NULL;
	}
	return (buf->content);
}