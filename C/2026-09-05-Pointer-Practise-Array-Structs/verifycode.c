#include <stdio.h>
#include <string.h>
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

#define CHECK_PTR(actual, expected, msg)                        \
    do                                                          \
    {                                                           \
        tests_run++;                                            \
        if ((const void *)(actual) == (const void *)(expected)) \
        {                                                       \
            tests_passed++;                                     \
        }                                                       \
        else                                                    \
        {                                                       \
            printf("    FAIL: %s -- pointer mismatch\n", msg);  \
        }                                                       \
    } while (0)


/* ===== "run" case: basic sum / scale / find / move, chained ===== */
void test_pointers_run(void)
{
    printf("  test_pointers_run\n");

    int a[] = {2, 4, 6, 8, 10};

    int run_sum = sum_range(a + 1, a + 4);
    CHECK_INT(run_sum, *(a+1) + *(a+2) + *(a+3), "sum_range(a+1, a+4) over {2,4,6,8,10}");

    scale_range(a + 2, a + 5, 3);
    int expected_after_scale[] = {2, 4, 18, 24, 30};
    for (int i = 0; i < 5; i++)
    {
        CHECK_INT(*(a+2) + *(a+3) + *(a+4), *(expected_after_scale+2) + *(expected_after_scale+3) + *(expected_after_scale+4), "a[i] after scale_range(a+2, a+5, 3)");
    }

    const int *pos = find_first(a, a + 5, 24);
    int idx = (int)(pos - a);
    CHECK_INT(idx, 3, "find_first(a, a+5, 24) -- index of match");

    Player p = {5, -2, "Rogue"};
    move_player(&p, -3, 7);
    CHECK_INT(p.x, 2, "move_player: x after dx=-3");
    CHECK_INT(p.y, 5, "move_player: y after dy=7");
}


/* ===== "submit" case: empty range, negatives, not-found, chained scale+sum ===== */
void test_pointers_submit(void)
{
    printf("  test_pointers_submit\n");

    int b[] = {0, -1, -2, -3, -4, -5};

    int s1 = sum_range(b, b);
    CHECK_INT(s1, 0, "sum_range(b, b) -- empty range");

    int s2 = sum_range(b + 1, b + 4);
    CHECK_INT(s2, -6, "sum_range(b+1, b+4) over negative values");

    int c[] = {5, 5, 7, 5, 9, 5};
    const int *f1 = find_first(c, c + 6, 7);
    int idx1 = (int)(f1 - c);
    CHECK_INT(idx1, 2, "find_first(c, c+6, 7) -- first match index");

    const int *f2 = find_first(c, c + 6, 42);
    CHECK_PTR(f2, c + 6, "find_first(c, c+6, 42) -- target absent, must return end");

    int d[] = {1, 2, 3, 4, 5, 6, 7};
    scale_range(d + 1, d + 6, 2);
    int total = sum_range(d, d + 7);
    CHECK_INT(total, 48, "sum_range(d, d+7) after scale_range(d+1, d+6, 2)");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for sum_range / scale_range / find_first / move_player\n\n");

    test_pointers_run();
    test_pointers_submit();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}