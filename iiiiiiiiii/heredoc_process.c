#include "shell.h"

int	heredoc_append_line(t_heredoc_buffer *buf)
{
	size_t	line_len;

	line_len = ft_strlen(buf->line);
	buf->line_with_nl = gb_malloc(line_len + 2);
	if (!buf->line_with_nl)
		return (0);
	ft_strcpy(buf->line_with_nl, buf->line);
	buf->line_with_nl[line_len] = '\n';
	buf->line_with_nl[line_len + 1] = '\0';
	buf->new_content = gb_malloc(ft_strlen(buf->content)
			+ ft_strlen(buf->line_with_nl) + 1);
	if (!buf->new_content)
	{
		gc_free(buf->line_with_nl);
		return (0);
	}
	ft_strcpy(buf->new_content, buf->content);
	ft_strcat(buf->new_content, buf->line_with_nl);
	gc_free(buf->content);
	gc_free(buf->line_with_nl);
	buf->content = buf->new_content;
	return (1);
}