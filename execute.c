/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 05:57:09 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/31 18:11:32 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "lexer/shell.h"

typedef struct s_exec_data 
{
    int in_fd;
    int pipefd[2];
    int *pids;
    int i;
    int pids_size;
    char **env;
    int last_exit_status;
	int original_stdin;
    int original_stdout;
	int return_status;
} t_exec_data;

typedef struct s_heredoc 
{
	char *content;
    size_t len;
} t_heredoc;
typedef struct s_heredoc_buffer 
{
	char *content;
	char *line;
	char *line_with_nl;
	char *new_content;
	size_t delimiter_len;
} t_heredoc_buffer;

typedef struct s_reader 
{
    char *buf;
    size_t size;
    size_t pos;
} t_reader;
char *ft_strcpy(char *dest, const char *src)
{
    size_t i;
	
	i = 0;
    while (src[i])
	{
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char *ft_strcat(char *dest, const char *src) 
{
    size_t i;
    size_t j;
	
	j = 0;
	i = 0;
    while (dest[i])
        i++;
    while (src[j])
	{
        dest[i + j] = src[j];
        j++;
    }
    dest[i + j] = '\0';
    return (dest);
}
void child_process_exec(t_parser *cmd, t_exec_data *data);

char *copy_dir(const char *dir, int len)
{
    char *res;
    int i;

	i = 0;
    res = malloc(len + 1);
    if (!res)
		return (NULL);
    while (i < len)
	{
		res[i] = dir[i];
		i++;
	}
    res[len] = '\0';
    return (res);
}

char *join_path(char *dir, char *file)
{
    size_t dlen;
    size_t flen;
	char *res;

	flen = ft_strlen(file);
	dlen = ft_strlen(dir);
    res = malloc(dlen + flen + 2);
    if (!res)
		return (NULL);
    ft_memcpy(res, dir, dlen);
    res[dlen] = '/';
    ft_memcpy(res + dlen + 1, file, flen);
    res[dlen + flen + 1] = '\0';
    return (res);
}

int is_exec_other(char *path)
{
    if (access(path, F_OK) != 0)
		return (0);
    if (access(path, X_OK) != 0)
		return (0);
    return (1);
}

char *check_cmd_in_dir(const char *dir_start, int len, char *cmd)
{
	char *result;
	char *dir;

    dir = copy_dir(dir_start, len);
    if (!dir)
		return (NULL);
    result = join_path(dir, cmd);
    free(dir);
    if (is_exec_other(result))
		return (result);
    free(result);
    return (NULL);
}

char *search_in_path(char *cmd, char *path)
{
    char *start;
    char *finish;
    char *found;
	int len;

	start = path;
	finish = path;
    while (1)
	{
        if (*start == ':' || *start == '\0') 
		{
            len = start - finish;
            if (len > 0)
			{
                found = check_cmd_in_dir(finish, len, cmd);
                if (found) return found;
            }
            if (*start == '\0')
				break;
            finish = start + 1;
        }
        start++;
    }
    return (NULL);
}
char *find_executable(char *cmd) 
{
	char *path;

    if (is_exec_other(cmd))
		return (ft_strdup(cmd));
    path = getenv("PATH");
    if (!path)
		return (NULL);
    return (search_in_path(cmd, path));
}


void redir_in(t_redirection *redir)
{
	int fd;

    fd = open(redir->filename, O_RDONLY);
    if (fd < 0)
	{
        perror("open <");
        exit(1);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
}

void redir_out(t_redirection *redir)
{
	int fd;

    fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0)
	{
        perror("open >");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}
void redir_append(t_redirection *redir)
{
	int fd;

    fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd < 0)
	{
        perror("open >>");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}
char *expand_buf(t_reader *r, size_t new_size)
{
    char *new_buf;
	size_t i;

	new_buf = malloc(new_size);
    if (!new_buf)
        return NULL;
    i = 0;
    while (i < r->pos) 
	{
        new_buf[i] = r->buf[i];
        i++;
    }
    free(r->buf);
    r->buf = new_buf;
    r->size = new_size;
    return (r->buf);
}
int add_char(t_reader *r, char c)
{
	size_t new_size;

    if (r->pos + 1 >= r->size)
	{
        new_size = r->size * 2;
        if (new_size < r->size + 1)
        	new_size = r->size + 1;
        if (!expand_buf(r, new_size))
            return (0);
    }
    r->buf[r->pos++] = c;
    return (1);
}
char *ft_readline_loop(t_reader *r)
{
    char c;
    ssize_t n;
    while ((n = read(STDIN_FILENO, &c, 1)) > 0)
	{
        if (c == '\n')
            break;
        if (!add_char(r, c))
		{
            free(r->buf);
            return NULL;
        }
    }
    if (n <= 0 && r->pos == 0)
	{
        free(r->buf);
        return NULL;
    }
    if (r->pos + 1 >= r->size && !expand_buf(r, r->pos + 1))
        return NULL;
    r->buf[r->pos] = '\0';
    return (r->buf);
}
char *ft_readline(char *prompt)
{
    t_reader r;
    r.size = 1;
    r.pos = 0;
    r.buf = malloc(r.size);
    if (!r.buf)
        return NULL;
    if (prompt)
        write(STDOUT_FILENO, prompt, ft_strlen(prompt));
    return (ft_readline_loop(&r));
}
int heredoc_append_line(t_heredoc_buffer *buf)
{
    size_t line_len;
	
	line_len = ft_strlen(buf->line);
    buf->line_with_nl = malloc(line_len + 2);
    if (!buf->line_with_nl)
        return 0;
    ft_strcpy(buf->line_with_nl, buf->line);
    buf->line_with_nl[line_len] = '\n';
    buf->line_with_nl[line_len + 1] = '\0';
    buf->new_content = malloc(ft_strlen(buf->content) + ft_strlen(buf->line_with_nl) + 1);
    if (!buf->new_content) 
	{
        free(buf->line_with_nl);
        return 0;
    }
    ft_strcpy(buf->new_content, buf->content);
    ft_strcat(buf->new_content, buf->line_with_nl);
    free(buf->content);
    free(buf->line_with_nl);
    buf->content = buf->new_content;
    return (1);
}
char *readline_loop(t_heredoc_buffer *buf,const char *delimiter)
{
	while (1) 
	{
		buf->line = ft_readline("> ");
		if (!buf->line)
			break;
		if ((size_t)ft_strlen(buf->line) == buf->delimiter_len && 
			ft_strcmp(buf->line, delimiter) == 0)
		{
			free(buf->line);
			break;
		}
		if (!heredoc_append_line(buf)) 
		{
			perror("Satır eklenemedi");
			free(buf->line);
			free(buf->content);
			return NULL;
		}
		free(buf->line);
		buf->line = NULL;
	}
	return (NULL);
}
char *read_single_heredoc_block(char *delimiter)
{
	t_heredoc_buffer buf;
    if (!delimiter || *delimiter == '\0') 
	{
        fprintf(stderr, "Hata: Heredoc ayirici kelimesi belirtilmedi.\n");
        return NULL;
    }
    buf.content = strdup("");
    buf.line = NULL;
    buf.line_with_nl = NULL;
    buf.new_content = NULL;
    buf.delimiter_len = ft_strlen(delimiter);
    if (!buf.content)
	{
        perror("Heredoc bellek ayirma basarisiz");
        return NULL;
    }
	readline_loop(&buf,delimiter);
    return buf.content;
}

char *remove_quotes_exec(char *str)
{
    if (!str)
        return NULL;
    
    int len = ft_strlen(str);
    if (len == 0)
        return str;
    
    // Önce quote'ların doğru eşleşip eşleşmediğini kontrol et
    int i = 0;
    char in_quote = 0;
    int quote_count_single = 0;
    int quote_count_double = 0;
    
    // Quote sayısını ve eşleşmesini kontrol et
    while (i < len)
    {
        if (str[i] == '"' && in_quote != '\'')
        {
            if (in_quote == '"')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '"';
            quote_count_double++;
        }
        else if (str[i] == '\'' && in_quote != '"')
        {
            if (in_quote == '\'')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '\'';
            quote_count_single++;
        }
        i++;
    }
    
    // Eğer quote açık kalmışsa veya tek sayıda quote varsa hata
    if (in_quote != 0 || (quote_count_single % 2 != 0) || (quote_count_double % 2 != 0))
    {
        printf("bash: syntax error: unclosed quotes\n");
        return NULL;
    }
    
    // Quote'ları kaldır
    char *result = malloc(len + 1);
    if (!result)
        return str;
    
    i = 0;
    int j = 0;
    in_quote = 0;
    
    while (i < len)
    {
        char current = str[i];
        
        if (current == '"' || current == '\'')
        {
            if (in_quote == 0)
            {
                in_quote = current;
            }
            else if (in_quote == current)
            {
                in_quote = 0;
            }
            else
            {
                result[j++] = current;
            }
        }
        else
        {
            result[j++] = current;
        }
        i++;
    }
    
    result[j] = '\0';
    
    if (j == len)
    {
        free(result);
        return str;
    }
    
    return result;
}
int ft_h_built(t_redirection *current_redir,char *heredoc_content,int *last_heredoc_fd, size_t *heredoc_len)
{
	int pipefd[2];
	heredoc_content = read_single_heredoc_block(current_redir->filename);
	if (!heredoc_content)
	{
		if (*last_heredoc_fd != -2 && *last_heredoc_fd != -1) 
			close(*last_heredoc_fd);
		return -1;
	}
	*heredoc_len = ft_strlen(heredoc_content);
	if (pipe(pipefd) == -1)
	{
		perror("heredoc pipe");
		free(heredoc_content); 
		if (*last_heredoc_fd != -2 && *last_heredoc_fd != -1)
			close(*last_heredoc_fd);
		return (-1);
	}
	write(pipefd[1], heredoc_content, *heredoc_len);
	close(pipefd[1]); 
	free(heredoc_content); 
	heredoc_content = NULL; 
	*last_heredoc_fd = pipefd[0]; 
	return (0);
}
int h_loop(t_parser *cmd,char *heredoc_content,int *last_heredoc_fd,size_t *heredoc_len)
{
	t_redirection *current_redir;

	current_redir = cmd->redirs;
	while (current_redir) 
	{
		if (current_redir->type == REDIR_HEREDOC)
		{
			if (heredoc_content)
			{
				free(heredoc_content);
				heredoc_content = NULL;
			}
			if (*last_heredoc_fd != -2 && *last_heredoc_fd != -1)
			{
				close(*last_heredoc_fd);
				*last_heredoc_fd = -1; 
			}
			if(ft_h_built(current_redir,heredoc_content,last_heredoc_fd,heredoc_len) == -1)
				return (-1);
		}
		current_redir = current_redir->next;
	}
return (0);
}
int process_heredocs(t_parser *cmd)
{
    int last_heredoc_fd;
    size_t heredoc_len;
    char *heredoc_content;

	heredoc_content = NULL;
	heredoc_len = 0;
	last_heredoc_fd = -2;
	if(h_loop(cmd,heredoc_content,&last_heredoc_fd,&heredoc_len) == -1)
		return (-1);
    return (last_heredoc_fd);
}
void setup_file_redirections(t_parser *cmd)
{
    t_redirection *current_redir;

    current_redir = cmd->redirs;
    while (current_redir)
	{
        if (current_redir->type == REDIR_IN)
            redir_in(current_redir);
        else if (current_redir->type == REDIR_OUT)
            redir_out(current_redir);
        else if (current_redir->type == REDIR_APPEND)
            redir_append(current_redir);
        current_redir = current_redir->next;
    }
}
void setup_redirections(t_redirection *redirs)
{
    while (redirs) 
    {
        char *filename = remove_quotes_exec(redirs->filename);
        
        if (redirs->type == REDIR_IN)
        {
            int fd = open(filename, O_RDONLY);
            if (fd < 0) 
            { 
                perror("open <"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 0);
            close(fd);
        }
        else if (redirs->type == REDIR_OUT)
        {
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (fd < 0) 
            { 
                perror("open >"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 1);
            close(fd);
        }
        else if (redirs->type == REDIR_APPEND)
        {
            int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
            if (fd < 0) 
            { 
                perror("open >>"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 1);
            close(fd);
        }
        
        if (filename != redirs->filename)
            free(filename);
            
        redirs = redirs->next;
    }
}
int ft_builtin_2(t_parser *cmd, t_env **env_list,t_exec_data *data)
{
    (void)env_list; // echo için env_list gerekmiyor
    if (ft_strcmp(cmd->argv[0], "echo") == 0) 
    {
       int saved_stdout = -1;
       if (cmd->redirs) 
       {
           saved_stdout = dup(1);
           setup_redirections(cmd->redirs);
       }
       built_echo(cmd);
       if (saved_stdout != -1) 
       {
           dup2(saved_stdout, 1);
           close(saved_stdout);
       }
       return 1;
    }
    data->last_exit_status = 0;
    return (0);
}

int ft_builtin(t_parser *cmd, t_exec_data *data ,t_env **env_list)
{
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 0;
        
    if (ft_strcmp(cmd->argv[0], "exit") == 0)
    {
        builtin_exit(cmd);
        //data->last_exit_status = 0;
        return 1;
    }
    if (ft_strcmp(cmd->argv[0], "cd") == 0) 
    {
        data->last_exit_status = built_cd(cmd, env_list);
        return 1;
    }
    if (ft_strcmp(cmd->argv[0], "pwd") == 0)
    {
        data->last_exit_status = builtin_pwd();
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "export") == 0)
    {
        data->last_exit_status = builtin_export(cmd, env_list);
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "unset") == 0)
    {
        data->last_exit_status = builtin_unset(cmd, env_list);
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "env") == 0)
    {
        data->last_exit_status = builtin_env(cmd, *env_list);
        return 1;
    }
    if (ft_builtin_2(cmd, env_list,data))
        return 1;
    return 0;
}

int count_commands(t_parser *cmd)
{
	int count;

    count = 0;
    while (cmd)
	{
		count++;
        cmd = cmd->next;
    }
    return (count);
}
void ft_redir_ctrl(t_parser *cmd)
{
	t_redirection *redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == REDIR_IN)
			redir_in(redir);
		else if (redir->type == REDIR_OUT)
			redir_out(redir);
		else if (redir->type == REDIR_APPEND)
			redir_append(redir);
		else if (redir->type == REDIR_HEREDOC) 
		{
			if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != -2)
			{
				if (dup2(cmd->heredoc_fd, STDIN_FILENO) == -1)
				{
					perror("dup2 cmd->heredoc_fd");
					exit(1);
				}
				close(cmd->heredoc_fd); 
				cmd->heredoc_fd = -1; 
			}
		}
		redir = redir->next;
	}
}
void ft_exec_start(t_parser *cmd,t_exec_data *data,t_env *env_list)
{
	char *exec_path;
	if (ft_builtin(cmd,data,&env_list))
	{
		ft_builtin(cmd, data,&env_list);
		exit(data->last_exit_status);
	} 
	else 
	{
		exec_path = find_executable(cmd->argv[0]);
		if (!exec_path)
		{
			fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
			exit(127);
		}
		execve(exec_path, cmd->argv, &env_list);
		perror("execve");
		exit(1);
	}

}

void child_process_exec(t_parser *cmd, t_exec_data *data,t_env *env_list)
{

    // Child process için signal'ları default'a çeviricez önemli
    setup_child_signals();

    if (data->in_fd != STDIN_FILENO)
	{
        if (dup2(data->in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 in_fd");
			exit(1);
		}
        close(data->in_fd);
    }
    if (cmd->next)
	{
        if (dup2(data->pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 pipefd[1]");
			exit(1);
		}
        close(data->pipefd[0]);
        close(data->pipefd[1]);
    }
	ft_redir_ctrl(cmd);
	ft_exec_start(cmd,data,env_list);
}

int setup_pipe_if_needed(t_parser *cmd, t_exec_data *data)
{
    if (!cmd->next)
        return (0);
    if (pipe(data->pipefd) == -1) 
	{
        perror("pipe");
        return -1;
    }
    return 0;
}

int fork_pipe_exec(t_parser *cmd, t_exec_data *data,t_env **env_list)
{
    int pid;

    pid = fork();
    if (pid == -1) 
	{
        perror("fork");
        return -1;
    }
    if (pid == 0) 
	{ 
        child_process_exec(cmd, data,&env_list);
        exit(1);
    }
    return (pid);
}

int setup_and_fork(t_parser *cmd, t_exec_data *data,t_env **env_list)
{
    int pid;

    if (setup_pipe_if_needed(cmd, data) == -1) 
        return (-1);
    pid = fork_pipe_exec(cmd, data,env_list);
    if (pid == -1) 
	{
        if (cmd->next) 
		{
            close(data->pipefd[0]);
            close(data->pipefd[1]);
        }
        return -1;
    }

    if (data->i >= data->pids_size) 
	{
        printf("limit error\n");
        return (-1);
    }
    data->pids[data->i++] = pid;
    return (pid);
}

int process_command(t_parser *cmd, t_exec_data *data,t_env **env_list)
{
    if (!cmd->argv || !cmd->argv[0])
	{
		 if (cmd->redirs)
    	{
        setup_file_redirections(cmd);
        return (1);
   		}
	}
    if (setup_and_fork(cmd, data,env_list) == -1)
        return 0;
    if (data->in_fd != STDIN_FILENO)
        close(data->in_fd);
    if (cmd->next)
	{
        close(data->pipefd[1]);
        data->in_fd = data->pipefd[0];
    } 
	else
        data->in_fd = STDIN_FILENO;
    return 1;
}

void execute_loop(t_parser *cmd_list, t_exec_data *data,t_env **env_list)
{
    data->in_fd = STDIN_FILENO;
    t_parser *cmd;

    cmd = cmd_list;
    while (cmd)
	{
        if (!process_command(cmd, data,env_list))
        	break;
        cmd = cmd->next;
    }
    if (data->in_fd != STDIN_FILENO)
        close(data->in_fd);
}

void wait_pids(t_exec_data *data)
{
    int status;
    int j;
	
	j = 0;
    while (j < data->i) 
	{
        waitpid(data->pids[j], &status, 0);
        if (j == data->i - 1)
		{
            if (WIFEXITED(status))
                data->last_exit_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                data->last_exit_status = 128 + WTERMSIG(status);
        }
        j++;
    }
}
void data_handle(t_exec_data *data, t_parser *cmd_list,int *pid_len, char **env) 
{
	*pid_len = count_commands(cmd_list);
	data->pids_size = *pid_len;
    data->pids = NULL;
    data->i = 0;
    data->env = env;
    data->in_fd = STDIN_FILENO;
    data->last_exit_status = 0;
}
int heredoc_fd_error(t_parser *cmd_list,t_exec_data *data,t_parser *current_cmd)
{
	t_parser *tmp_cmd;

	dup2(data->original_stdin, STDIN_FILENO);
	close(data->original_stdin);
	dup2(data->original_stdout, STDOUT_FILENO);
	close(data->original_stdout);
	tmp_cmd = cmd_list;
	while(tmp_cmd != current_cmd->next) 
	{
		if (tmp_cmd && tmp_cmd->heredoc_fd != -1 && tmp_cmd->heredoc_fd != -2) 
		{
			close(tmp_cmd->heredoc_fd);
			tmp_cmd->heredoc_fd = -1;
		}
		tmp_cmd = tmp_cmd->next;
	}
	return (1);

}
int heredoc_handle(t_parser *cmd_list,t_exec_data *data)
{
	t_parser *current_cmd;

	current_cmd = cmd_list;
	while (current_cmd) 
	{
		current_cmd->heredoc_fd = process_heredocs(current_cmd);
		if (current_cmd->heredoc_fd == -1) 
		{
			heredoc_fd_error(cmd_list,data,current_cmd);
			return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0); 
}
int n_next_or_built(t_parser *cmd_list, t_exec_data *data)
{
	if (cmd_list->heredoc_fd != -1 && cmd_list->heredoc_fd != -2) 
	{
		if (dup2(cmd_list->heredoc_fd, STDIN_FILENO) == -1) 
		{
			perror("dup2 heredoc for builtin");
			data->return_status = 1;
		}
		close(cmd_list->heredoc_fd);
		cmd_list->heredoc_fd = -1;
	}
	if (data->return_status == 0) 
	{
		setup_file_redirections(cmd_list);
		ft_builtin_call(cmd_list, data);
		
	}
	dup2(data->original_stdin, STDIN_FILENO);
	close(data->original_stdin);
	dup2(data->original_stdout, STDOUT_FILENO);
	close(data->original_stdout);
	return (data->last_exit_status);
	
}
int execute_and_pids(t_parser *cmd_list,t_exec_data *data,t_env **env_list)
{
	execute_loop(cmd_list, data,env_list);
	wait_pids(data);
	free(data->pids);
	return (0);
}
int ft_data_pids(t_parser *cmd_list, t_exec_data *data,t_env **env_list)
{
	t_parser *current_cmd;

	current_cmd = cmd_list;
	data->pids = malloc(sizeof(int) * data->pids_size);
	if (!data->pids) 
	{
		perror("pids error\n");
		current_cmd = cmd_list;
		while(current_cmd) 
		{
			if (current_cmd->heredoc_fd != -1 && current_cmd->heredoc_fd != -2) 
			{
				close(current_cmd->heredoc_fd);
				current_cmd->heredoc_fd = -1;
			}
			current_cmd = current_cmd->next;
		}
		dup2(data->original_stdin, STDIN_FILENO);
		close(data->original_stdin);
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
		return (1);
	}
	execute_and_pids(cmd_list,data,env_list);
	return (0);
}
void finish_fd(t_parser *cmd_list,t_exec_data *data)
{
	t_parser *current_cmd;

	current_cmd = cmd_list;
	while(current_cmd) 
	{
		if (current_cmd->heredoc_fd != -1 && current_cmd->heredoc_fd != -2) {
			close(current_cmd->heredoc_fd);
			current_cmd->heredoc_fd = -1;
		}
		current_cmd = current_cmd->next;
	}
	dup2(data->original_stdin, STDIN_FILENO);
	close(data->original_stdin);
	dup2(data->original_stdout, STDOUT_FILENO);
	close(data->original_stdout);
	
}

int execute_cmds(t_parser *cmd_list, t_env **env_list)
{
    t_exec_data data;
	int pid_len;
    data.return_status = 0;
	data.original_stdin = dup(STDIN_FILENO);
	data.original_stdout = dup(STDOUT_FILENO);
    if (data.original_stdin == -1 || data.original_stdout == -1) 
	{
		perror("dup failed");
        return 1;
    }
	if (heredoc_handle(cmd_list,&data))
		return (1);
    data_handle(&data, cmd_list,&pid_len, env_list);

    if (pid_len == 1 && ft_builtin(cmd_list,&data,env_list)) 
	{
		if(!n_next_or_built(cmd_list,&data))
			return (data.return_status);
    }
	if (ft_data_pids(cmd_list,&data,env_list))
		return (1);
	finish_fd(cmd_list,&data);
	return (data.last_exit_status);
}