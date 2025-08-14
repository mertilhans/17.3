#include "shell.h"

static t_token	*handle_redirection_out(t_tokenizer *tokenizer)
{
	if (lexer_peek(tokenizer) == '>')
	{
		lexer_advance(tokenizer);
		lexer_advance(tokenizer);
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	lexer_advance(tokenizer);
	return (create_token(TOKEN_REDIR_OUT, ">"));
}

static t_token	*handle_redirection_in(t_tokenizer *tokenizer)
{
	if (lexer_peek(tokenizer) == '<')
	{
		lexer_advance(tokenizer);
		lexer_advance(tokenizer);
		return (create_token(TOKEN_HEREDOC, "<<"));
	}
	lexer_advance(tokenizer);
	return (create_token(TOKEN_REDIR_IN, "<"));
}

static t_token	*tokenizer_get_next_token_2(t_tokenizer *tokenizer)
{
	if (tokenizer->current == '\0')
		return (create_token(TOKEN_EOF, NULL));
	if (tokenizer->current == '|')
	{
		lexer_advance(tokenizer);
		return (create_token(TOKEN_PIPE, "|"));
	}
	if (tokenizer->current == '<')
		return (handle_redirection_in(tokenizer));
	return (NULL);
}

t_token	*tokenizer_get_next_token(t_tokenizer *tokenizer)
{
	char	*word;
	t_token	*token;

	if (!tokenizer)
		return (NULL);
	ft_skip_space(tokenizer);
	if (tokenizer->current == '>')
		return (handle_redirection_out(tokenizer));
	token = tokenizer_get_next_token_2(tokenizer);
	if (token)
		return (token);
	word = extract_complex_word(tokenizer);
	if (word && ft_strlen(word) > 0)
		return (create_token(TOKEN_WORD, word));
	return (create_token(TOKEN_ERROR, NULL));
}

t_token	*tokenize_input(char *input)
{
	t_tokenizer	*tokenizer;
	t_token		*tokens;
	t_token		*current;
	t_token		*new_token;

	if (ft_tokenize_control(input))
		return (NULL);
	tokenizer = tokenizer_init(input);
	if (!tokenizer)
		return (NULL);
	tokens = NULL;
	current = NULL;
	while (1)
	{
		new_token = tokenizer_get_next_token(tokenizer);
		if (!new_token || new_token->type == TOKEN_EOF)
		{
			if (new_token)
				gc_free(new_token);
			break ;
		}
		if (!tokens)
		{
			tokens = new_token;
			current = tokens;
		}
		else
		{
			current->next = new_token;
			current = new_token;
		}
	}
	tokenizer_free(tokenizer);
	return (tokens);
}
