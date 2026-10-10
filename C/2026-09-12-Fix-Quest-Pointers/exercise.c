#include <stdlib.h>
#include "exercise.h"

quest_t* new_quest(const char* name, int reward) {
  quest_t *quest = malloc(sizeof(quest_t));

  if (quest == NULL) {
    return NULL;
  }
  quest->name = name;
  quest->reward = reward;
  quest->completed = 0;

  return quest;
}

void complete_quest(quest_t* quest) {
  if (quest == NULL) {
    return;
  }
  
  quest->completed = 1;
}

int quest_reward_if_completed(const quest_t* quest) {
  if (quest == NULL) {
    return 0;
  }

  if (quest->completed) {
    return quest->reward;
  }

  return 0;
}