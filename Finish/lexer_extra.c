#include "shell.h"

int ft_tokenize_control(char *input)
{
	char *current = input;
	while (*current)
	{
		if (*current == '|')
		{
			// Basit bir örnek: başta veya sonda pipe varsa hata döndür
			if (current == input || *(current + 1) == '\0')
				return 1;
		}
		current++;
	}
	return 0;
}
