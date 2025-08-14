#include "shell.h"

static char	*init_heredoc_buffer(t_heredoc_buffer *buf, char *delimiter)
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

static char	*readline_loop(t_heredoc_buffer *buf, const char *delimiter)
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
		if ((size_t)ft_strlen(buf->line) == buf->delimiter_len
			&& ft_strcmp(buf->line, delimiter) == 0)
		{
			gc_free(buf->line);
			break ;
		}
		if (!heredoc_append_line(buf))
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
		gc_free(buf.content);
		return (NULL);
	}
	return (result ? result : buf.content);
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
	if (!init_heredoc_buffer(&buf, delimiter))
	{
		set_last_exit_status(1);
		return (NULL);
	}
	result = readline_loop_expand(&buf, delimiter, env_list);
	if (!result && get_last_exit_status() == 130)
	{
		gc_free(buf.content);
		return (NULL);
	}
	return (result ? result : buf.content);
}

int	process_heredocs(t_parser *cmd, t_env *env_list)
{
	t_heredoc_data	h_data;
	t_redirection	*current_redir;
	int				pipefd[2];

	h_data.heredoc_content = NULL;
	h_data.heredoc_len = 0;
	h_data.last_heredoc_fd = -2;
	current_redir = cmd->redirs;
	while (current_redir)
	{
		if (current_redir->type == REDIR_HEREDOC)
		{
			if (h_data.last_heredoc_fd != -2 && h_data.last_heredoc_fd != -1)
			{
				close(h_data.last_heredoc_fd);
				h_data.last_heredoc_fd = -1;
			}
			if (current_redir->no_expand)
				h_data.heredoc_content = read_single_heredoc_block(
						current_redir->filename);
			else
				h_data.heredoc_content = read_heredoc_with_expand(
						current_redir->filename, env_list);
			if (!h_data.heredoc_content && get_last_exit_status() == 130)
				return (-1);
			if (!h_data.heredoc_content)
				h_data.heredoc_content = ft_strdup("");
			h_data.heredoc_len = ft_strlen(h_data.heredoc_content);
			if (pipe(pipefd) == -1)
			{
				perror("heredoc pipe");
				gc_free(h_data.heredoc_content);
				set_last_exit_status(1);
				return (-1);
			}
			if (h_data.heredoc_len > 0)
				write(pipefd[1], h_data.heredoc_content, h_data.heredoc_len);
			close(pipefd[1]);
			gc_free(h_data.heredoc_content);
			h_data.last_heredoc_fd = pipefd[0];
		}
		current_redir = current_redir->next;
	}
	return (h_data.last_heredoc_fd);
}