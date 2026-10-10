#pragma once

typedef struct
{
    const char *name;
    int reward;
    int completed; // 0 = not completed, 1 = completed
} quest_t;

quest_t *new_quest(const char *name, int reward);
void complete_quest(quest_t *quest);
int quest_reward_if_completed(const quest_t *quest);