#include <stdio.h>
#include <stddef.h>

int sum_ints(const int *data, size_t len);
int find_value(const int *data, size_t len, int value);
size_t cstring_len(const char *s);

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

/* ===== the two cases from the pasted main.c, reproduced in plain C ===== */

void test_run_sum_find_len(void)
{
    printf("  test_run_sum_find_len\n");

    {
        int data[] = {3, -2, 7, 4};
        int actual = sum_ints(data, 4);
        CHECK_INT(actual, 12, "sum_ints([3,-2,7,4], 4)");
    }
    {
        int data[] = {10, 20, 30, 40, 50};
        int actual = find_value(data, 5, 30);
        CHECK_INT(actual, 2, "find_value([10,20,30,40,50], 5, 30)");
    }
    {
        const char *s = "guild_master"; /* 12 chars */
        size_t actual = cstring_len(s);
        CHECK_INT((int)actual, 12, "cstring_len(\"guild_master\")");
    }
}

void test_submit_edge_and_happy(void)
{
    printf("  test_submit_edge_and_happy\n");

    {
        int data[1] = {0};
        int actual = sum_ints(data, 0);
        CHECK_INT(actual, 0, "sum_ints([], 0)");
    }
    {
        int data[] = {1, 1, 2, 3, 5, 8};
        int actual = find_value(data, 6, 7);
        CHECK_INT(actual, -1, "find_value([1,1,2,3,5,8], 6, 7) -- not found");
    }
    {
        const char *s = "";
        size_t actual = cstring_len(s);
        CHECK_INT((int)actual, 0, "cstring_len(\"\")");
    }
    {
        int data[] = {5, 5, 5, 5, 5, 5};
        int sumActual = sum_ints(data, 6);
        CHECK_INT(sumActual, 30, "sum_ints([5,5,5,5,5,5], 6)");

        int idxActual = find_value(data, 6, 5);
        CHECK_INT(idxActual, 0, "find_value([5,5,5,5,5,5], 6, 5) -- first index, not last");
    }
}

/* ===== extra case beyond the original test file: a large length, to
   directly probe the int/size_t signedness mismatch the compiler
   warns about (see README) rather than just asserting it's fine ===== */

void test_large_length_boundary(void)
{
    printf("  test_large_length_boundary\n");

    static int big[70000];
    for (size_t i = 0; i < 70000; i++)
    {
        big[i] = 1;
    }

    int actual = sum_ints(big, 70000);
    CHECK_INT(actual, 70000, "sum_ints(70000-element array of 1s) -- exceeds a 16-bit int range");

    int idx = find_value(big, 70000, 1);
    CHECK_INT(idx, 0, "find_value(70000-element array, value=1) -- first index");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for sum_ints / find_value / cstring_len\n\n");

    test_run_sum_find_len();
    test_submit_edge_and_happy();
    test_large_length_boundary();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}