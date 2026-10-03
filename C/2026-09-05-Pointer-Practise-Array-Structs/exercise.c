#include "exercise.h"
#include <stdlib.h>

int sum_range(const int *begin, const int *end)
{
    if (begin == NULL || end == NULL || begin == end)
    {
        return 0;
    }

    int sum = 0;

    while (begin < end)
    {
        sum += *begin;
        begin++;
    }

    return sum;
}

void scale_range(int *begin, int *end, int factor)
{
    if (begin == NULL || end == NULL || begin == end)
    {
        return;
    }

    while (begin < end)
    {
        *begin *= factor;
        begin++;
    }
}

const int *find_first(const int *begin, const int *end, int target)
{

    while (begin < end)
    {
        if (*begin == target)
        {
            return begin;
        }
        begin++;
    }

    return end;
}

void move_player(Player *p, int dx, int dy)
{
    p->x += dx;
    p->y += dy;
}
