#ifndef ENV_H
#define ENV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct s_env
{
        char *key;
        char *value;
        struct s_env *next;
}       t_env;

#endif