#pragma once
#include <stddef.h>

typedef struct
{
    int min;
    int max;
    int sum;
} stat_t;

int compute_stats(const int *arr, size_t len, stat_t *out);
void clamp_in_place(int *arr, size_t len, int minv, int maxv);
