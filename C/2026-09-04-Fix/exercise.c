#include <stdlib.h>
#include <string.h>
#include "exercise.h"

snek_obj_t *snek_new_int(long value)
{
    snek_obj_t *obj = malloc(sizeof(snek_obj_t));
    if (!obj)
        return NULL;
    obj->type = SNEK_INT;
    obj->refcount = 1;
    obj->as.int_value = value;
    return obj;
}

snek_obj_t *snek_new_string(const char *value)
{
    if (!value)
        return NULL;
    snek_obj_t *obj = malloc(sizeof(snek_obj_t));
    if (!obj)
        return NULL;

    size_t len = strlen(value);
    char *data = malloc(len + 1);
    if (!data)
    {
        free(obj);
        return NULL;
    }

    memcpy(data, value, len + 1);

    obj->type = SNEK_STRING;
    obj->refcount = 1;
    obj->as.str.data = data;
    obj->as.str.length = (int)len;
    return obj;
}

void snek_free(snek_obj_t *obj)
{
    if (!obj)
        return;
    if (obj->type == SNEK_STRING)
    {
        free(obj->as.str.data);
    }
    free(obj);
}

snek_obj_t *snek_add(snek_obj_t *left, snek_obj_t *right)
{
    if (!left || !right)
    {
        return NULL;
    }

    if (left->type == SNEK_INT && right->type == SNEK_INT)
    {
        long value = left->as.int_value + right->as.int_value;
        snek_obj_t *new_obj = snek_new_int(value);
        return new_obj;
    }

    if (left->type == SNEK_STRING && right->type == SNEK_STRING)
    {
        int left_len = left->as.str.length;
        int right_len = right->as.str.length;

        int new_len = left_len + right_len + 1;
        char *new_string = malloc(new_len);
        if (new_string == NULL)
        {
            return NULL;
        }
        memcpy(new_string, left->as.str.data, left_len);
        memcpy(new_string + left_len, right->as.str.data, right_len + 1);

        snek_obj_t *obj_string = snek_new_string(new_string);

        free(new_string);

        return obj_string;
    }

    return NULL;
}
