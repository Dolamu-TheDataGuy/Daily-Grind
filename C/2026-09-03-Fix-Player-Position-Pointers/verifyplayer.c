#include <stdio.h>
#include <stddef.h>

/* ===== task.c — your solution, unmodified ===== */

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

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

/* ===== check macros =====*/
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

#define CHECK_PTR(actual, expected, msg)                       \
    do                                                         \
    {                                                          \
        tests_run++;                                           \
        if ((void *)(actual) == (void *)(expected))            \
        {                                                      \
            tests_passed++;                                    \
        }                                                      \
        else                                                   \
        {                                                      \
            printf("    FAIL: %s -- pointer mismatch\n", msg); \
        }                                                      \
    } while (0)

#define CHECK_NULL(ptr, msg)                                \
    do                                                      \
    {                                                       \
        tests_run++;                                        \
        if ((ptr) == NULL)                                  \
        {                                                   \
            tests_passed++;                                 \
        }                                                   \
        else                                                \
        {                                                   \
            printf("    FAIL: %s -- expected NULL\n", msg); \
        }                                                   \
    } while (0)

    
/* ===== the four cases from boot.dev's main.c, reproduced in plain C ===== */

void test_move_and_find_basic(void)
{
    printf("  test_move_and_find_basic\n");
    int positions[5] = {10, 20, 30, 40, 50};
    int length = 5;

    move_players(positions, length, 3);

    int expected_after[5] = {13, 23, 33, 43, 53};
    for (int i = 0; i < length; i++)
    {
        CHECK_INT(positions[i], expected_after[i],
                  "positions[i] after move_players(delta=3)");
    }

    int *found = find_player(positions, length, 33);
    CHECK_PTR(found, &positions[2], "find_player(positions, 5, 33)");
}

void test_move_negative_delta(void)
{
    printf("  test_move_negative_delta\n");
    int positions[4] = {5, 15, 25, 35};
    int length = 4;

    move_players(positions, length, -5);

    int expected_after[4] = {0, 10, 20, 30};
    for (int i = 0; i < length; i++)
    {
        CHECK_INT(positions[i], expected_after[i],
                  "positions[i] after move_players(delta=-5)");
    }
}

void test_find_not_found(void)
{
    printf("  test_find_not_found\n");
    int positions[3] = {100, 200, 300};
    int length = 3;

    int *found = find_player(positions, length, 999);
    CHECK_NULL(found, "find_player(positions, 3, 999) -- target absent");
}

void test_move_and_find_multiple(void)
{
    printf("  test_move_and_find_multiple\n");
    int positions[7] = {0, 10, 20, 30, 40, 50, 60};
    int length = 7;

    move_players(positions, length, 7);

    int expected_after[7] = {7, 17, 27, 37, 47, 57, 67};
    for (int i = 0; i < length; i++)
    {
        CHECK_INT(positions[i], expected_after[i],
                  "positions[i] after move_players(delta=7)");
    }

    /* two equal values -- find_player must return the FIRST match */
    positions[1] = 99;
    positions[4] = 99;

    int *found = find_player(positions, length, 99);
    CHECK_PTR(found, &positions[1],
              "find_player returns first match, not last");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for move_players / find_player\n\n");

    test_move_and_find_basic();
    test_move_negative_delta();
    test_find_not_found();
    test_move_and_find_multiple();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}