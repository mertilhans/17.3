#include "shell.h"

static ssize_t	read_char_from_stdin(char *ch)
{
	ssize_t	bytes_read;

	while (1)
	{
		bytes_read = read(STDIN_FILENO, ch, 1);
		if (bytes_read <= 0)
		{
			if (bytes_read == 0)
				return (0);
			else if (errno == EINTR)
			{
				errno = EINTR;
				return (-1);
			}
			else
				return (-1);
		}
		return (bytes_read);
	}
}

static char	*resize_line_buffer(char *line, size_t *line_size)
{
	char	*new_line;
	size_t	old_size;

	old_size = *line_size;
	*line_size *= 2;
	new_line = gb_malloc(*line_size);
	if (!new_line)
	{
		gc_free(line);
		return (NULL);
	}
	ft_memcpy(new_line, line, old_size);
	gc_free(line);
	return (new_line);
}

static char	*process_line_char(char ch, char *line, size_t *line_len,
	size_t *line_size)
{
	if (ch == '\n')
	{
		line[*line_len] = '\0';
		return (line);
	}
	if (ch == '\r')
		return (NULL);
	if (*line_len + 1 >= *line_size)
	{
		line = resize_line_buffer(line, line_size);
		if (!line)
			return (NULL);
	}
	line[(*line_len)++] = ch;
	return (NULL);
}

char	*heredoc_readline(const char *prompt)
{
	char	*line;
	size_t	line_size;
	size_t	line_len;
	ssize_t	bytes_read;
	char	ch;

	line_size = 128;
	line_len = 0;
	if (prompt)
		write(STDOUT_FILENO, prompt, ft_strlen((char *)prompt));
	line = gb_malloc(line_size);
	if (!line)
		return (NULL);
	while (1)
	{
		bytes_read = read_char_from_stdin(&ch);
		if (bytes_read <= 0)
		{
			if (bytes_read == 0 && line_len == 0)
			{
				gc_free(line);
				return (NULL);
			}
			if (errno == EINTR)
			{
				gc_free(line);
				errno = EINTR;
				return (NULL);
			}
			gc_free(line);
			return (NULL);
		}
		if (process_line_char(ch, line, &line_len, &line_size))
			return (line);
	}
}
