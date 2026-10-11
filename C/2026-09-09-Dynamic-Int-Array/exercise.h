#pragma once

#include <stddef.h>

typedef struct
{
    int *data;
    size_t length;
    size_t capacity;
} DynamicIntArray;

void dia_init(DynamicIntArray *arr);
int dia_append(DynamicIntArray *arr, int value);
int dia_insert(DynamicIntArray *arr, size_t index, int value);
void dia_free(DynamicIntArray *arr);
