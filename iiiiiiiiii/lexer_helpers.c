#include <stddef.h>
#include "shell.h"

t_token *create_token(t_token_type type, char *value)
{
	(void)type;
	(void)value;
	return NULL;
}

t_tokenizer *tokenizer_init(char *input)
{
	(void)input;
	return NULL;
}

void tokenizer_free(t_tokenizer *tokenizer)
{
	(void)tokenizer;
}
