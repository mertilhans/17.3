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

// DÜZELTME: Boşluklu değerleri doğru işlemek için yeniden yazıldı
char	*export_build_value(t_parser *cmd, int *i, char *value)
{
	char	*full_value;
	int		j;
	int		total_len;
	int		value_len;

	if (!value)
		return (NULL);
	
	value_len = ft_strlen(value);
	total_len = value_len;
	
	// Sonraki argümanları kontrol et ve toplam uzunluğu hesapla
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		total_len += ft_strlen(cmd->argv[j]) + 1; // +1 for space
		j++;
	}
	
	// Bellek ayır
	full_value = gb_malloc(total_len + 1);
	if (!full_value)
		return (NULL);
	
	// İlk değeri kopyala
	ft_strcpy(full_value, value);
	
	// Sonraki argümanları birleştir
	j = *i + 1;
	while (cmd->argv[j] && !ft_strchr(cmd->argv[j], '='))
	{
		ft_strcat(full_value, " ");
		ft_strcat(full_value, cmd->argv[j]);
		j++;
	}
	
	*i = j - 1; // Index'i güncelle
	return (full_value);
}

// DÜZELTME: Güvenli string işleme ile yeniden yazıldı
void	export_process_keyvalue(t_parser *cmd, int *i, t_env **env_list)
{
	t_export	**export_list;
	char		*current_arg;
	char		*eq_pos;
	char		*key;
	char		*value;
	char		*full_value;

	export_list = get_export_list();
	current_arg = cmd->argv[*i];
	eq_pos = ft_strchr(current_arg, '=');
	
	if (!eq_pos)
		return;
		
	*eq_pos = '\0'; // Geçici olarak böl
	key = current_arg;
	value = eq_pos + 1;
	
	// Boş değer kontrolü
	if (!value || ft_strlen(value) == 0)
	{
		set_export_value(export_list, key, "");
		set_env_value(env_list, key, "");
	}
	else
	{
		// Çoklu argüman varsa birleştir
		if (cmd->argv[*i + 1] && !ft_strchr(cmd->argv[*i + 1], '='))
		{
			full_value = export_build_value(cmd, i, value);
			if (full_value)
			{
				set_export_value(export_list, key, full_value);
				set_env_value(env_list, key, full_value);
				gc_free(full_value);
			}
			else
			{
				set_export_value(export_list, key, value);
				set_env_value(env_list, key, value);
			}
		}
		else
		{
			set_export_value(export_list, key, value);
			set_env_value(env_list, key, value);
		}
	}
	
	*eq_pos = '='; // Restore original string
}


// ana export fonksiyonu - DÜZELTME: Güvenli işleme eklendi
void	builtin_export(t_parser *cmd, t_env **env_list)
{
	int			i;
	t_export	**export_list;
	char		*current_arg;
	char		*eq_pos;
	char		*identifier;
	char		original_char;

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
			// Geçici olarak böl
			original_char = *eq_pos;
			*eq_pos = '\0';
			identifier = current_arg;
			
			if (!validate_identifier(identifier))
			{
				printf("bash: export: `%s': not a valid identifier\n", identifier);
				*eq_pos = original_char; // Restore
				i++;
				continue;
			}
			
			*eq_pos = original_char; // Restore
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