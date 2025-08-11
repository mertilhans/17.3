#include "shell.h"

void	close_all_fds_except_std(t_parser *cmd)
{
	t_parser	*cmd_list;

	cmd_list = cmd;
	while (cmd_list)
	{
		if (cmd_list->heredoc_fd != -2 && cmd_list->heredoc_fd != -1)
		{
			close(cmd_list->heredoc_fd);
			cmd_list->heredoc_fd = -1;
		}
		cmd_list = cmd_list->next;
	}
}

char	*heredoc_readline(const char *prompt)
{
	char	*line;
	size_t	line_size;
	size_t	line_len;
	ssize_t	bytes_read;
	char	ch;

	if (prompt)
		write(STDOUT_FILENO, prompt, strlen(prompt));
	line_size = 128;
	line_len = 0;
	line = gb_malloc(line_size);
	if (!line)
		return (NULL);
	while (1)
	{
		bytes_read = read(STDIN_FILENO, &ch, 1);
		if (bytes_read <= 0)
		{
			if (bytes_read == 0 && line_len == 0)
			{
				gc_free(line);
				return (NULL);
			}
			else if (errno == EINTR)
			{
				gc_free(line);
				errno = EINTR;
				return (NULL);
			}
			else
			{
				gc_free(line);
				return (NULL);
			}
		}
		if (ch == '\n')
		{
			line[line_len] = '\0';
			return (line);
		}
		if (ch != '\r')
		{
			if (line_len + 1 >= line_size)
			{
				line_size *= 2;
				line = gb_malloc(line_size);
				if (!line)
					return (NULL);
			}
			line[line_len++] = ch;
		}
	}
}

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

void	ft_putendl_fd(char *str, int fd)
{
	if (!str)
		return ;
	while (*str)
		write(fd, str++, 1);
}

int	is_builtin(t_parser *cmd)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (strcmp(cmd->argv[0], "exit") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "cd") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "pwd") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "echo") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "export") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "unset") == 0)
		return (1);
	if (strcmp(cmd->argv[0], "env") == 0)
		return (1);
	return (0);
}