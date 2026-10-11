#include <stdio.h>
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

static void check_array_equal(const int *got, const int *exp, size_t len,
                              const char *msg)
{
    size_t i;
    for (i = 0; i < len; i++)
    {
        CHECK_INT(got[i], exp[i], msg);
    }
}

/* ===== the four cases from the pasted main.c, reproduced in plain C ===== */

void test_compute_stats_run(void)
{
    printf("  test_compute_stats_run\n");

    int data[] = {4, -1, 7, 3};
    stat_t s;
    int ok = compute_stats(data, 4, &s);

    CHECK_INT(ok, 1, "len=4 -> return 1");
    CHECK_INT(s.min, -1, "[4,-1,7,3] -> min should be -1");
    CHECK_INT(s.max, 7, "[4,-1,7,3] -> max should be 7");
    CHECK_INT(s.sum, 13, "[4,-1,7,3] -> sum should be 13");
}

void test_clamp_in_place_run(void)
{
    printf("  test_clamp_in_place_run\n");

    int nums[] = {-5, 0, 5, 10, 15};
    int expected[] = {0, 0, 5, 10, 10};
    clamp_in_place(nums, 5, 0, 10);
    check_array_equal(nums, expected, 5, "[-5,0,5,10,15], min=0, max=10");
}

void test_compute_stats_submit_edge_and_happy(void)
{
    printf("  test_compute_stats_submit_edge_and_happy\n");

    /* Edge: empty array */
    int empty[1] = {0}; /* size-1 placeholder; len passed as 0 so content is unused */
    stat_t s1;
    int ok1 = compute_stats(empty, 0, &s1);
    CHECK_INT(ok1, 0, "[] -> return 0");
    CHECK_INT(s1.min, 0, "[] -> min should be 0");
    CHECK_INT(s1.max, 0, "[] -> max should be 0");
    CHECK_INT(s1.sum, 0, "[] -> sum should be 0");

    /* Happy: mixed values */
    int data[] = {12, -3, 8, 8, 20, -10, 5};
    stat_t s2;
    int ok2 = compute_stats(data, 7, &s2);
    CHECK_INT(ok2, 1, "[12,-3,8,8,20,-10,5] -> return 1");
    CHECK_INT(s2.min, -10, "[12,-3,8,8,20,-10,5] -> min should be -10");
    CHECK_INT(s2.max, 20, "[12,-3,8,8,20,-10,5] -> max should be 20");
    CHECK_INT(s2.sum, 40, "[12,-3,8,8,20,-10,5] -> sum should be 40");
}

void test_clamp_in_place_submit(void)
{
    printf("  test_clamp_in_place_submit\n");

    int all_low[] = {-9, -2, -1};
    int expected_low[] = {0, 0, 0};
    clamp_in_place(all_low, 3, 0, 5);
    check_array_equal(all_low, expected_low, 3, "[-9,-2,-1], min=0, max=5");

    int mixed[] = {12, -3, 8, 8, 20, -10, 5};
    int expected_mixed[] = {10, 0, 8, 8, 10, 0, 5};
    clamp_in_place(mixed, 7, 0, 10);
    check_array_equal(mixed, expected_mixed, 7, "[12,-3,8,8,20,-10,5], min=0, max=10");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for compute_stats / clamp_in_place\n\n");

    test_compute_stats_run();
    test_clamp_in_place_run();
    test_compute_stats_submit_edge_and_happy();
    test_clamp_in_place_submit();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}