#include <stddef.h>

void move_players(int *positions, int length, int delta)
{

    for (int i = 0; i < length; i++)
    {
        *positions += delta;
        positions++;
    }
}

int *find_player(int *positions, int length, int target)
{
    if (positions == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        int value = *(positions + i);
        if (value == target)
        {
            return positions + i;
        }
    }
    return NULL;
}