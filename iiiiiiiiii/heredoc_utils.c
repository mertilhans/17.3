#include "shell.h"

static char	*expand_heredoc_line_impl(char *line, t_env *env_list)
{
	char	*expanded;

	if (!line)
		return (NULL);
	expanded = expand_heredoc_line(line, env_list);
	if (expanded)
		return (expanded);
	else
		return (ft_strdup(line));
}

int	heredoc_append_expanded(t_heredoc_buffer *buf, t_env *env_list)
{
	char	*expanded_line;
	char	*old_line;
	int		result;

	old_line = buf->line;
	expanded_line = expand_heredoc_line_impl(buf->line, env_list);
	buf->line = expanded_line;
	result = heredoc_append_line(buf);
	buf->line = old_line;
	if (expanded_line != old_line)
		gc_free(expanded_line);
	return (result);
}
