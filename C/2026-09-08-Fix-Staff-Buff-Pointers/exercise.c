#include <stdlib.h>

int *first_stat_over(int *stats, int length, int threshold)
{
    if (stats == NULL || length <= 0)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        if (stats[i] > threshold)
        {
            return &stats[i];
        }
    }
    return NULL;
}

int *copy_under_limit(int *stats, int length, int limit, int *out_length)
{
    if (stats == NULL || length <= 0)
    {
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < length; i++)
    {
        if (stats[i] <= limit)
        {
            count++;
        }
    }

    if (count == 0)
    {
        *out_length = 0;
        return NULL;
    }

    int *result = malloc(count * sizeof(int));
    if (result == NULL)
    {
        *out_length = 0;
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < length; i++)
    {
        if (stats[i] <= limit)
        {
            result[j] = stats[i];
            j++;
        }
    }

    *out_length = count;

    return result;
}
