#include "exercise.h"

int compute_stats(const int *arr, size_t len, stat_t *out)
{
    if (len == 0)
    {
        out->min = 0;
        out->max = 0;
        out->sum = 0;
        return 0;
    }

    int sum = 0;
    int *p = arr;
    int min = *p;
    int max = *p;

    for (int i = 0; i < len; i++, p++)
    {
        if (*p > max)
        {
            max = *p;
        }
        if (*p < min)
        {
            min = *p;
        }
        sum += *p;
    }
    out->min = min;
    out->max = max;
    out->sum = sum;
    return 1;
}

void clamp_in_place(int *arr, size_t len, int minv, int maxv)
{
    if (len == 0)
    {
        return;
    }

    int *p = arr;

    for (int i = 0; i < len; i++, p++)
    {
        if (*p < minv)
        {
            *p = minv;
        }

        if (*p > maxv)
        {
            *p = maxv;
        }
    }
}
