#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "exercise.h"

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_INT(actual, expected, msg)                    \
    do                                                      \
    {                                                       \
        tests_run++;                                        \
        if ((actual) == (expected))                         \
        {                                                   \
            tests_passed++;                                 \
        }                                                   \
        else                                                \
        {                                                   \
            printf("    FAIL: %s -- expected %d, got %d\n", \
                   msg, (int)(expected), (int)(actual));    \
        }                                                   \
    } while (0)

#define CHECK_PTR_NOT_NULL(actual, msg)                                   \
    do                                                                    \
    {                                                                     \
        tests_run++;                                                      \
        if ((actual) != NULL)                                             \
        {                                                                 \
            tests_passed++;                                               \
        }                                                                 \
        else                                                              \
        {                                                                 \
            printf("    FAIL: %s -- expected non-NULL, got NULL\n", msg); \
        }                                                                 \
    } while (0)

#define CHECK_STR(actual, expected, msg)                            \
    do                                                              \
    {                                                               \
        tests_run++;                                                \
        if ((actual) != NULL && strcmp((actual), (expected)) == 0)  \
        {                                                           \
            tests_passed++;                                         \
        }                                                           \
        else                                                        \
        {                                                           \
            printf("    FAIL: %s -- expected \"%s\", got \"%s\"\n", \
                   msg, expected, (actual) ? (actual) : "(null)");  \
        }                                                           \
    } while (0)

/* ===== the four cases from the pasted main.c, reproduced in plain C ===== */

void test_basic_quest(void)
{
    printf("  test_basic_quest\n");

    quest_t *q = new_quest("Find the amulet", 100);

    CHECK_PTR_NOT_NULL(q, "new_quest(\"Find the amulet\", 100)");
    if (q)
    {
        CHECK_STR(q->name, "Find the amulet", "q->name");
        CHECK_INT(q->reward, 100, "q->reward");
        CHECK_INT(q->completed, 0, "new quest starts not completed");

        int reward_before = quest_reward_if_completed(q);
        CHECK_INT(reward_before, 0, "uncompleted quest -- reward 0");

        complete_quest(q);
        CHECK_INT(q->completed, 1, "quest marked completed");

        int reward_after = quest_reward_if_completed(q);
        CHECK_INT(reward_after, 100, "completed quest -- full reward");
    }

    free(q);
}

void test_multiple_quests(void)
{
    printf("  test_multiple_quests\n");

    quest_t *q1 = new_quest("Clear the rats", 25);
    quest_t *q2 = new_quest("Escort the merchant", 75);

    CHECK_PTR_NOT_NULL(q1, "q1");
    CHECK_PTR_NOT_NULL(q2, "q2");

    if (q1 && q2)
    {
        complete_quest(q2);

        int r1 = quest_reward_if_completed(q1);
        int r2 = quest_reward_if_completed(q2);

        CHECK_INT(r1, 0, "q1 uncompleted -- reward 0");
        CHECK_INT(r2, 75, "q2 completed -- reward 75");
    }

    free(q1);
    free(q2);
}

void test_null_and_unfinished(void)
{
    printf("  test_null_and_unfinished\n");

    quest_t *q_null = NULL;

    complete_quest(q_null); /* must not crash */
    int r_null = quest_reward_if_completed(q_null);
    CHECK_INT(r_null, 0, "NULL quest -- reward 0");

    quest_t *q = new_quest("Defend the village", 200);
    CHECK_PTR_NOT_NULL(q, "new_quest(\"Defend the village\", 200)");

    if (q)
    {
        int r_unfinished = quest_reward_if_completed(q);
        CHECK_INT(r_unfinished, 0, "not-completed quest -- reward 0");
    }

    free(q);
}

void test_big_happy_path(void)
{
    printf("  test_big_happy_path\n");

    quest_t *q1 = new_quest("Rescue the prince", 300);
    quest_t *q2 = new_quest("Gather herbs", 40);
    quest_t *q3 = new_quest("Slay the dragon", 1000);

    CHECK_PTR_NOT_NULL(q1, "q1");
    CHECK_PTR_NOT_NULL(q2, "q2");
    CHECK_PTR_NOT_NULL(q3, "q3");

    if (q1 && q2 && q3)
    {
        complete_quest(q1);
        complete_quest(q3);

        int r1 = quest_reward_if_completed(q1);
        int r2 = quest_reward_if_completed(q2);
        int r3 = quest_reward_if_completed(q3);
        int total = r1 + r2 + r3;

        CHECK_INT(r1, 300, "q1 completed -- 300");
        CHECK_INT(r2, 0, "q2 not completed -- 0");
        CHECK_INT(r3, 1000, "q3 completed -- 1000");
        CHECK_INT(total, 1300, "total collected reward");
    }

    free(q1);
    free(q2);
    free(q3);
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for new_quest / complete_quest / quest_reward_if_completed\n\n");

    test_basic_quest();
    test_multiple_quests();
    test_null_and_unfinished();
    test_big_happy_path();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}