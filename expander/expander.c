// Düzenlenmiş ve tekrar eden kodlar kaldırıldı
#include "../shell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int g_last_exit_status = 0;

int is_valid_char(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
}

char *expand_variables(char *str, t_env *env_list)
{
    char *res;
    int res_len = 0;
    int res_cap = 1024;
    int i = 0;
    if (!str)
        return NULL;
    res = malloc(res_cap);
    if (!res)
        return NULL;
    res[0] = '\0';
    while (str[i])
    {
        if (str[i] == '$')
        {
            char *expanded = NULL;
            // int start = i; // unused variable
            expanded = handle_dollar(str, &i, env_list);
            if (expanded)
            {
                int add_len = ft_strlen(expanded);
                if (res_len + add_len >= res_cap)
                {
                    res_cap *= 2;
                    char *new_res = realloc(res, res_cap); // şimdilik test için malloc + dup yapçam
                    if (!new_res)
                    {
                        gc_free(res);
                        gc_free(expanded);
                        return NULL;
                    }
                    res = new_res;
                }
                ft_memcpy(res + res_len, expanded, add_len);
                res_len += add_len;
                res[res_len] = '\0';
                gc_free(expanded);
            }
        }
        else
        {
            if (res_len + 1 >= res_cap)
            {
                res_cap *= 2;
                char *new_res = realloc(res, res_cap);
                if (!new_res)
                {
                    free(res);
                    return NULL;
                }
                res = new_res;
            }
            res[res_len++] = str[i++];
            res[res_len] = '\0';
        }
    }
    return res;
}

char *handle_dollar(char *str, int *pos, t_env *env_list)
{
    int i = *pos + 1;
    if (str[i] == '?')
    {
        *pos = i + 1;
        return ft_itoa(g_last_exit_status);
    }
    if (str[i] == '$')
    {
        *pos = i + 1;
        return ft_itoa(getpid());
    }
    if (is_valid_char(str[i]))
        return handle_simple(str, pos, env_list);
    
    // Geçerli bir değişken bulunamadığında veya $ tek başına kaldığında
    // boş bir string döndür.
    (*pos)++;
    return ft_strdup("");
}

char *handle_simple(const char *str, int *i, t_env *env_list)
{
    int len = 0;
    char *var_name;
    
    (*i)++;
    while (is_valid_char(str[*i + len]))
        len++;
    
    if (len == 0)
    {
        (*i)++;
        return ft_strdup("");
    }
    
    var_name = strndup(str + *i, len);
    *i += len;
    
    t_env *env_var = find_env(env_list, var_name);
    
    if (env_var == NULL) // boş string bulamadıysak mk
    {
        char *result = ft_strdup("");
        gc_free(var_name);
        return result;
    }
    
    free(var_name);
    
    if (env_var->value == NULL)
        return ft_strdup("");
    else
        // Değişkenin değeri varsa, o değeri döndür.
        return ft_strdup(env_var->value);
}

static int	digit_count(int n)
{
	int	len;

	len = 0;
	if (n < 0)
	{
		len++;
		n = -n;
	}
	while (n != 0)
	{
		n = n / 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char		*str;
	int			len;
	long int	num;

	num = n;
	len = digit_count(num);
	if (n == 0)
		return (ft_strdup("0"));
	str = (char *)malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	str[len] = '\0';
	while (num > 0)
	{
		str[--len] = (num % 10) + '0';
		num = num / 10;
	}
	return (str);
}


char *push_char_res(char *res, char c, int *res_len, int *res_cap)
{
    if (*res_len + 1 >= *res_cap)
    {
        *res_cap *= 2;
        char *new_res = realloc(res, *res_cap);
        if (!new_res)
            return res;
        res = new_res;
    }
    res[(*res_len)++] = c;
    res[*res_len] = '\0';
    return res;
}

static char *append_string_res(char *result, char *str, int *res_len, int *res_cap)
{
    // int i = 0; // unused variable
    int add_len = strlen(str);
    if (*res_len + add_len >= *res_cap)
    {
        *res_cap = (*res_len + add_len) * 2;
        char *new_res = realloc(result, *res_cap);
        if (!new_res)
            return result;
        result = new_res;
    }
    memcpy(result + *res_len, str, add_len);
    *res_len += add_len;
    result[*res_len] = '\0';
    return result;
}

// static char *process_expansion(char *result, char *str, int *i, t_env *env, char quote_char, int *res_len, int *res_cap)
// {
//     char *expanded;
//     if (str[*i] == '$' && quote_char != '\'')
//     {
//         expanded = handle_dollar(str, i, env);
//         if (expanded)
//         {
//             result = append_string_res(result, expanded, res_len, res_cap);
//             free(expanded);
//         }
//     }
//     else
//         result = push_char_res(result, str[(*i)++], res_len, res_cap);
//     return result;
// }

// char *expand_with_quotes(char *str, t_env *env)
// {
//     int res_len = 0, res_cap = 1024, i = 0;
//     char quote_char = 0;
//     char *result = malloc(res_cap);
//     int remove_quotes = 0; 
//     if (!result)
//         return NULL;
//     result[0] = '\0';
//     int len = ft_strlen(str);
//     if (len >= 2 && ((str[0] == '"' && str[len-1] == '"') || (str[0] == '\'' && str[len-1] == '\'')))
//     {
//         remove_quotes = 1;
//         quote_char = str[0];
//         i = 1; // İlk quote'u atla
//         len--; // Son quote'u da atla
//     }
//     while (i < len)
//     {
//         if (!remove_quotes && is_quote_char(str[i], quote_char))
//         {
//             quote_char = update_quote_state(str[i], quote_char);
//             result = push_char_res(result, str[i++], &res_len, &res_cap);
//         }
//         else
//         {
//             result = process_expansion(result, str, &i, env, remove_quotes ? quote_char : 0, &res_len, &res_cap);
//         }
//     }
//     return result;
// }
char *expand_with_quotes(char *str, t_env *env)
{
    if (!str)
        return NULL;
        
    int res_len = 0, res_cap = 1024, i = 0;
    char quote_char = 0;
    char *result = malloc(res_cap);
    if (!result)
        return NULL;
    result[0] = '\0';
    
    while (str[i])
    {
        if (str[i] == '\'' || str[i] == '"')
        {
            char current_quote = str[i];
            if (quote_char == 0)
            {
                // Quote başlangıcı - quote karakterini atla
                quote_char = current_quote;
                i++;
            }
            else if (quote_char == current_quote)
            {
                // Quote bitişi - quote karakterini atla
                quote_char = 0;
                i++;
            }
            else
            {
                // Farklı quote içindeyken - karakteri ekle
                result = push_char_res(result, str[i++], &res_len, &res_cap);
            }
        }
        else if (str[i] == '$' && quote_char != '\'')
        {
            // Variable expansion (single quote içinde değilse)
            char *expanded = handle_dollar(str, &i, env);
            if (expanded)
            {
                result = append_string_res(result, expanded, &res_len, &res_cap);
                free(expanded);
            }
        }
        else
        {
            // Normal karakter
            result = push_char_res(result, str[i++], &res_len, &res_cap);
        }
    }
    
    return result;
}