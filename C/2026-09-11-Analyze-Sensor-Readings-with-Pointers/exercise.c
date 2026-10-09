#include <stddef.h>

int *find_first_above(int *readings, int length, int limit)
{
    for (int i = 0; i < length; i++, readings++)
    {
        if (*readings > limit)
        {
            return readings;
        }
    }
    return NULL;
}

int count_between(int *start, int *end)
{
    if (start == NULL || end == NULL)
    {
        return 0;
    }
    int count = 0;
    while (start < end)
    {
        count += 1;
        start++;
    }

    return count;
}

void add_offset(int *start, int *end, int offset)
{
    if (start == NULL || end == NULL)
    {
        return;
    }

    while (start < end)
    {
        *start += offset;
        start++;
    }
}
