#include <stdio.h>
#include <stddef.h>

int *find_first_above(int *readings, int length, int limit);
int count_between(int *start, int *end);
void add_offset(int *start, int *end, int offset);

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

#define CHECK_NULL(actual, msg)                                           \
    do                                                                    \
    {                                                                     \
        tests_run++;                                                      \
        if ((actual) == NULL)                                             \
        {                                                                 \
            tests_passed++;                                               \
        }                                                                 \
        else                                                              \
        {                                                                 \
            printf("    FAIL: %s -- expected NULL, got non-NULL\n", msg); \
        }                                                                 \
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

/* ===== the five cases from the pasted main.c, reproduced in plain C ===== */

void test_pointers_run_basic(void)
{
    printf("  test_pointers_run_basic\n");

    int readings[] = {5, 7, 12, 3, 9};
    int length = 5;

    int *p = find_first_above(readings, length, 10);
    CHECK_PTR_NOT_NULL(p, "find_first_above({5,7,12,3,9}, 5, 10)");
    if (p)
        CHECK_INT(*p, 12, "first value > 10");

    int *start = &readings[1]; /* 7 */
    int *end = &readings[4];   /* just before 9 */
    int c = count_between(start, end);
    CHECK_INT(c, 3, "count_between(readings[1], readings[4])");

    add_offset(start, end, 1);
    /* readings should now be {5, 8, 13, 4, 9} */
    CHECK_INT(readings[0], 5, "readings[0] unchanged (before range)");
    CHECK_INT(readings[1], 8, "readings[1] = 7+1");
    CHECK_INT(readings[2], 13, "readings[2] = 12+1");
    CHECK_INT(readings[3], 4, "readings[3] = 3+1");
    CHECK_INT(readings[4], 9, "readings[4] unchanged (at end pointer)");
}

void test_pointers_run_no_match(void)
{
    printf("  test_pointers_run_no_match\n");

    int readings[] = {1, 2, 3, 4};
    int length = 4;

    int *p = find_first_above(readings, length, 10);
    CHECK_NULL(p, "find_first_above({1,2,3,4}, 4, 10) -- no value > 10");

    int *start = &readings[0];
    int *end = &readings[0];
    int c = count_between(start, end);
    CHECK_INT(c, 0, "count_between(start, start) -- empty range");

    add_offset(start, end, 100);
    CHECK_INT(readings[0], 1, "add_offset on empty range -- no modification");
}

void test_pointers_submit_edge_and_partial(void)
{
    printf("  test_pointers_submit_edge_and_partial\n");

    int readings[] = {15, 20, 25, 5, 30};
    int length = 5;

    int *p = find_first_above(readings, length, 15);
    CHECK_PTR_NOT_NULL(p, "find_first_above({15,20,25,5,30}, 5, 15)");
    if (p)
        CHECK_INT(*p, 20, "first value > 15");

    int *start = &readings[1]; /* 20 */
    int *end = &readings[5];   /* just past 30 */
    int c = count_between(start, end);
    CHECK_INT(c, 4, "count_between(readings[1], readings[5])");

    add_offset(start, end, -5);
    int expected[] = {15, 15, 20, 0, 25};
    for (int i = 0; i < length; i++)
    {
        CHECK_INT(readings[i], expected[i], "readings[i] after add_offset(..., -5)");
    }
}

void test_pointers_submit_all_elements(void)
{
    printf("  test_pointers_submit_all_elements\n");

    int readings[] = {0, 0, 0, 0};
    int length = 4;

    int *p = find_first_above(readings, length, -1);
    CHECK_PTR_NOT_NULL(p, "find_first_above({0,0,0,0}, 4, -1)");
    if (p)
        CHECK_INT(*p, 0, "first value > -1");
    CHECK_PTR(p, &readings[0], "pointer must point to first element");

    int *start = &readings[0];
    int *end = &readings[4];
    int c = count_between(start, end);
    CHECK_INT(c, 4, "count_between over the whole array");

    add_offset(start, end, 10);
    for (int i = 0; i < length; i++)
    {
        CHECK_INT(readings[i], 10, "readings[i] after add_offset(..., 10) over whole array");
    }
}

void test_pointers_submit_large_limit(void)
{
    printf("  test_pointers_submit_large_limit\n");

    int readings[] = {100, 200, 300, 400, 500};
    int length = 5;

    int *p = find_first_above(readings, length, 450);
    CHECK_PTR_NOT_NULL(p, "find_first_above({100,200,300,400,500}, 5, 450)");
    if (p)
        CHECK_INT(*p, 500, "first value > 450");

    int *start = &readings[2]; /* 300 */
    int *end = &readings[4];   /* just before 500 */
    int c = count_between(start, end);
    CHECK_INT(c, 2, "count_between(readings[2], readings[4])");

    add_offset(start, end, 50);
    CHECK_INT(readings[0], 100, "readings[0] unchanged (before range)");
    CHECK_INT(readings[1], 200, "readings[1] unchanged (before range)");
    CHECK_INT(readings[2], 350, "readings[2] = 300+50");
    CHECK_INT(readings[3], 450, "readings[3] = 400+50");
    CHECK_INT(readings[4], 500, "readings[4] unchanged (at end pointer)");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for find_first_above / count_between / add_offset\n\n");

    test_pointers_run_basic();
    test_pointers_run_no_match();
    test_pointers_submit_edge_and_partial();
    test_pointers_submit_all_elements();
    test_pointers_submit_large_limit();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}