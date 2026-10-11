#include <stdlib.h>
#include <stddef.h>

#include "exercise.h"

void dia_init(DynamicIntArray *arr)
{
    arr->data = NULL;
    arr->length = 0;
    arr->capacity = 0;
}

int dia_append(DynamicIntArray *arr, int value)
{

    if (arr->length == arr->capacity)
    {
        size_t new_capacity;

        if (arr->capacity == 0)
        {
            new_capacity = 4;
        }
        else
        {
            new_capacity = arr->capacity * 2;
        }

        int *new_data = realloc(arr->data, new_capacity * sizeof(int));

        if (new_data == NULL)
        {
            return 0;
        }

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    arr->data[arr->length] = value;
    arr->length++;
    return 1;
}

int dia_insert(DynamicIntArray *arr, size_t index, int value)
{
    if (index > arr->length)
    {
        return 0;
    }

    if (arr->length == arr->capacity)
    {
        size_t new_capacity;

        if (arr->capacity == 0)
        {
            new_capacity = 4;
        }
        else
        {
            new_capacity = arr->capacity * 2;
        }

        int *new_data = realloc(arr->data, new_capacity * sizeof(int));

        if (new_data == NULL)
        {
            return 0;
        }

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    for (size_t i = arr->length; i > index; i--)
    {
        arr->data[i] = arr->data[i - 1];
    }

    arr->data[index] = value;
    arr->length++;
    return 1;
}

void dia_free(DynamicIntArray *arr)
{
    if (arr->data != NULL)
    {
        // This is wrong
        // for (int i=0; i < arr->length; i++) {
        //   free(arr->data[i]);
        // }
        free(arr->data);
    }
    arr->data = NULL;
    arr->length = 0;
    arr->capacity = 0;
}
