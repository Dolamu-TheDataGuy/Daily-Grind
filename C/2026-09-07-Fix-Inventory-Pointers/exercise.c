#include <stdlib.h>
#include "exercise.h"

Inventory *create_inventory(const char **names, const int *quantities, int count)
{
    if (count <= 0 || names == NULL || quantities == NULL)
    {
        return NULL;
    }

    Inventory *inv = malloc(sizeof(Inventory));
    if (inv == NULL)
    {
        return NULL;
    }
    inv->size = count;

    inv->items = malloc(sizeof(Item) * count);
    if (inv->items == NULL)
    {
        free(inv);
        return NULL;
    }

    for (int i = 0; i < count; i++)
    {
        Item item;
        int j = 0;
        if (names != NULL && names[i] != NULL)
        {
            while (names[i][j] != '\0' && j < 31)
            {
                item.name[j] = names[i][j];
                j++;
            }
        }
        item.name[j] = '\0';
        item.quantity = quantities[i];

        inv->items[i] = item;
    }
    return inv;
}

void free_inventory(Inventory *inv)
{
    if (inv == NULL)
    {
        return;
    }

    free(inv->items);
    free(inv);
}
