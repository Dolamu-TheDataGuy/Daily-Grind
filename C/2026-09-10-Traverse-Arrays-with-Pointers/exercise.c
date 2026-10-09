#include <stddef.h>

int sum_ints(const int *data, size_t len)
{
    if (len == 0)
    {
        return 0;
    }

    int sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

int find_value(const int *data, size_t len, int value)
{
    for (int i = 0; i < len; i++)
    {
        if (data[i] == value)
        {
            return i;
        }
    }

    return -1;
}

size_t cstring_len(const char *s)
{
    size_t count = 0;

    while (*s != '\0')
    {
        count++;
        s++;
    }

    return count;
}
