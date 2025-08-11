#include "shell.h"

t_tokenizer	*tokenizer_init(char *input)
{
	t_tokenizer	*tokenizer;

	if (!input)
		return (NULL);
	tokenizer = gb_malloc(sizeof(t_tokenizer));
	if (!tokenizer)
		return (NULL);
	tokenizer->input = ft_strdup(input);
	if (!tokenizer->input)
	{
		gc_free(tokenizer);
		return (NULL);
	}
	tokenizer->pos = 0;
	tokenizer->len = ft_strlen(input);
	if (tokenizer->len > 0)
		tokenizer->current = input[0];
	else
		tokenizer->current = '\0';
	return (tokenizer);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = gb_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = ft_strdup(value);
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

int	main(int ac, char **av, char **env)
{
	t_env	*env_list;

	(void)ac;
	(void)av;
	set_last_exit_status(0);
	env_list = initialize_shell(env);
	shell_loop(env_list, env);
	rl_clear_history();
	env_gb_free_all();
	gb_free_all();
	return (get_last_exit_status());
}