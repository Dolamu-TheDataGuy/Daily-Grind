#pragma once

typedef struct Player
{
    int x;
    int y;
    char name[16];
} Player;

int sum_range(const int *begin, const int *end);
void scale_range(int *begin, int *end, int factor);
const int *find_first(const int *begin, const int *end, int target);
void move_player(Player *p, int dx, int dy);
