#pragma once

typedef struct Item
{
    char name[32];
    int quantity;
} Item;

typedef struct Inventory
{
    Item *items;
    int size;
} Inventory;

Inventory *create_inventory(const char **names, const int *quantities, int count);
void free_inventory(Inventory *inv);
