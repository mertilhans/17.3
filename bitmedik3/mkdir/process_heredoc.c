#include "shell.h"

static int	create_heredoc_pipe(t_heredoc_data *data)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("heredoc pipe");
		gc_free(data->heredoc_content);
		set_last_exit_status(1);
		return (-1);
	}
	if (data->heredoc_len > 0)
		write(pipefd[1], data->heredoc_content, data->heredoc_len);
	close(pipefd[1]);
	gc_free(data->heredoc_content);
	data->last_heredoc_fd = pipefd[0];
	return (pipefd[0]);
}

static int	heredoc_append_expanded(t_heredoc_buffer *buf, t_env *env_list)
{
	char	*expanded_line;
	char	*old_line;
	int		result;

	old_line = buf->line;
	expanded_line = expand_heredoc_line(buf->line, env_list);
	if (!expanded_line)
		expanded_line = ft_strdup(buf->line);
	buf->line = expanded_line;
	result = heredoc_append_line(buf);
	buf->line = old_line;
	if (expanded_line != old_line)
		gc_free(expanded_line);
	return (result);
}

static char	*readline_loop_expand(t_heredoc_buffer *buf,
								const char *delimiter, t_env *env_list)
{
	heredoc_signals();
	while (1)
	{
		errno = 0;
		buf->line = heredoc_readline("> ");
		if (!buf->line)
		{
			if (errno == EINTR)
			{
				write(STDOUT_FILENO, "", 0);
				set_last_exit_status(130);
				setup_interactive_signals();
				return (NULL);
			}
			printf("bash: warning: here-document at line 1 ");
			printf("delimited by end-of-file (wanted `%s')\n", delimiter);
			set_last_exit_status(0);
			return (buf->content);
		}
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
	setup_interactive_signals();
	return (buf->content);
}

char	*read_heredoc_with_expand(char *delimiter, t_env *env_list)
{
	t_heredoc_buffer	buf;
	char				*result;

	if (!delimiter || *delimiter == '\0')
	{
		printf("Heredoc delimiter error\n");
		set_last_exit_status(1);
		return (NULL);
	}
	buf.content = ft_strdup("");
	buf.line = NULL;
	buf.line_with_nl = NULL;
	buf.new_content = NULL;
	buf.delimiter_len = ft_strlen(delimiter);
	if (!buf.content)
	{
		perror("Heredoc malloc error");
		set_last_exit_status(1);
		return (NULL);
	}
	result = readline_loop_expand(&buf, delimiter, env_list);
	if (!result && get_last_exit_status() == 130)
	{
		gc_free(buf.content);
		return (NULL);
	}
	setup_interactive_signals();
	return (result);
}

int	ft_h_built_expand(t_redirection *current_redir,
					t_heredoc_data *data, t_env *env_list)
{
	if (current_redir->no_expand)
		data->heredoc_content = read_single_heredoc_block(
				current_redir->filename);
	else
		data->heredoc_content = read_heredoc_with_expand(
				current_redir->filename, env_list);
	if (!data->heredoc_content && get_last_exit_status() == 130)
		return (-1);
	if (!data->heredoc_content)
		data->heredoc_content = ft_strdup("");
	data->heredoc_len = ft_strlen(data->heredoc_content);
	return (create_heredoc_pipe(data));
}