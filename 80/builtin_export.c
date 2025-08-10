#include "shell.h"

// DÜZELTME: Identifier validation fonksiyonları
static int	is_valid_identifier_start(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static int	is_valid_identifier_char(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
			(c >= '0' && c <= '9') || c == '_');
}

static int	validate_identifier(char *identifier)
{
	int	i;

	if (!identifier || identifier[0] == '\0')
		return (0);
	if (identifier[0] == '$')
		return (0);
	if (!is_valid_identifier_start(identifier[0]))
		return (0);
	
	i = 1;
	while (identifier[i])
	{
		if (!is_valid_identifier_char(identifier[i]))
			return (0);
		i++;
	}
	return (1);
}

// export listesinin hepsini yazdır abi
void	export_print_all(t_export **export_list)
{
	t_export	*current;

	current = *export_list;
	while (current)
	{
		printf("declare -x %s", current->key);
		if (current->value != NULL)
			printf("=\"%s\"", current->value);
		printf("\n");
		current = current->next;
	}
}

// DÜZELTME: Güvenli string işleme ile yeniden yazıldı
void	export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
{
	t_export	**export_list;
	char		*current_arg;
	char		*eq_pos;
	char		*value;
	char		*key_copy;
	char		*value_copy;

	export_list = get_export_list();
	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	
	if (!eq_pos)
		return;
		
	// Key'i güvenli şekilde ayır
	key_copy = ft_strndup(current_arg, eq_pos - current_arg);
	if (!key_copy)
		return;
		
	value = eq_pos + 1;
	
	// Value'yu güvenli şekilde kopyala
	if (!value || ft_strlen(value) == 0)
	{
		value_copy = ft_strdup("");
	}
	else
	{
		value_copy = ft_strdup(value);
	}
	
	if (!value_copy)
	{
		gc_free(key_copy);
		return;
	}
	
	// Export ve env listelerine ekle
	set_export_value(export_list, key_copy, value_copy);
	set_env_value(env_list, key_copy, value_copy);
	
	// Belleği temizle
	gc_free(key_copy);
	gc_free(value_copy);
}

// ana export fonksiyonu - DÜZELTME: Güvenli işleme eklendi
void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	char		*current_arg;
	char		*eq_pos;
	char		*identifier;

	i = 1;
	export_list = get_export_list();
	if (!cmd->argv[1])
	{
		export_print_all(export_list);
		return ;
	}
	
	while (cmd->argv[i])
	{
		current_arg = cmd->argv[i];
		eq_pos = ft_strchr(current_arg, '=');
		
		if (eq_pos)
		{
			// Identifier'ı güvenli şekilde ayır
			identifier = ft_strndup(current_arg, eq_pos - current_arg);
			if (!identifier)
			{
				i++;
				continue;
			}
			
			if (!validate_identifier(identifier))
			{
				printf("bash: export: `%s': not a valid identifier\n", identifier);
				gc_free(identifier);
				i++;
				continue;
			}
			
			gc_free(identifier);
			export_process_keyvalue(cmd, &i, env_list);
		}
		else
		{
			// Sadece key, değer yok
			if (!validate_identifier(current_arg))
			{
				printf("bash: export: `%s': not a valid identifier\n", current_arg);
				i++;
				continue;
			}
			set_export_value(export_list, current_arg, NULL);
		}
		i++;
	}
}

// değişkeni hem env hem export listesinden kaldır abi
void	builtin_unset(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;

	i = 1;
	export_list = get_export_list();
	if (!cmd->argv[1])
	{
		printf("unset: not enough arguments\n");
		return ;
	}
	while (cmd->argv[i])
	{
		if (!validate_identifier(cmd->argv[i]))
		{
			printf("bash: unset: `%s': not a valid identifier\n", cmd->argv[i]);
			i++;
			continue;
		}
		unset_env_value(env_list, cmd->argv[i]);
		unset_export_value(export_list, cmd->argv[i]);
		i++;
	}
}