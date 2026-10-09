#include <stdio.h>
#include <stdlib.h>

/* This task has no exercise.h -- boot.dev's own main.c forward-declares
   both functions directly instead of including a header, so this file
   does the same. */
int *first_stat_over(int *stats, int length, int threshold);
int *copy_under_limit(int *stats, int length, int limit, int *out_length);

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_INT(actual, expected, msg)                                   \
    do {                                                                   \
        tests_run++;                                                       \
        if ((actual) == (expected)) {                                      \
            tests_passed++;                                                \
        } else {                                                           \
            printf("    FAIL: %s -- expected %d, got %d\n",                \
                   msg, (int)(expected), (int)(actual));                   \
        }                                                                  \
    } while (0)

#define CHECK_PTR_NOT_NULL(actual, msg)                                    \
    do {                                                                   \
        tests_run++;                                                       \
        if ((actual) != NULL) {                                            \
            tests_passed++;                                                \
        } else {                                                           \
            printf("    FAIL: %s -- expected non-NULL, got NULL\n", msg);  \
        }                                                                  \
    } while (0)

#define CHECK_NULL(actual, msg)                                            \
    do {                                                                   \
        tests_run++;                                                       \
        if ((actual) == NULL) {                                            \
            tests_passed++;                                                \
        } else {                                                           \
            printf("    FAIL: %s -- expected NULL, got non-NULL\n", msg);  \
        }                                                                  \
    } while (0)

/* ===== the four cases from the pasted main.c, reproduced in plain C ===== */

void test_pointers_run_basic(void) {
    printf("  test_pointers_run_basic\n");

    int stats[] = {5, 12, 7, 20};

    int *p = first_stat_over(stats, 4, 10);
    CHECK_PTR_NOT_NULL(p, "first_stat_over(stats, 4, 10)");
    if (p) CHECK_INT(*p, 12, "first stat over 10");

    int *p2 = first_stat_over(stats, 4, 19);
    CHECK_PTR_NOT_NULL(p2, "first_stat_over(stats, 4, 19)");
    if (p2) CHECK_INT(*p2, 20, "first stat over 19");
}

void test_pointers_run_copy(void) {
    printf("  test_pointers_run_copy\n");

    int stats[] = {5, 12, 7, 20};
    int out_len = 0;
    int *copied = copy_under_limit(stats, 4, 10, &out_len);

    CHECK_PTR_NOT_NULL(copied, "copy_under_limit(stats, 4, 10, &out_len)");
    CHECK_INT(out_len, 2, "out_len after copy_under_limit(..., 10, ...)");
    if (copied && out_len == 2) {
        CHECK_INT(copied[0], 5, "copied[0]");
        CHECK_INT(copied[1], 7, "copied[1]");
    }

    free(copied);
}

void test_pointers_submit_first(void) {
    printf("  test_pointers_submit_first\n");

    int stats[] = {3, 3, 3, 3};
    int *p = first_stat_over(stats, 4, 5);
    CHECK_NULL(p, "first_stat_over({3,3,3,3}, 4, 5) -- no value > 5");

    int stats2[] = {1, 4, 6, 6, 2};
    int *p2 = first_stat_over(stats2, 5, 4);
    CHECK_PTR_NOT_NULL(p2, "first_stat_over({1,4,6,6,2}, 5, 4)");
    if (p2) CHECK_INT(*p2, 6, "first stat over 4 in {1,4,6,6,2}");
}

void test_pointers_submit_copy_edges(void) {
    printf("  test_pointers_submit_copy_edges\n");

    int stats_all_low[] = {2, 2, 2};
    int out_len1 = -1;
    int *copy1 = copy_under_limit(stats_all_low, 3, 5, &out_len1);

    CHECK_PTR_NOT_NULL(copy1, "copy_under_limit({2,2,2}, 3, 5, ...) -- all under limit");
    CHECK_INT(out_len1, 3, "out_len1");
    if (copy1) {
        for (int i = 0; i < out_len1; i++) {
            CHECK_INT(copy1[i], 2, "copy1[i] all 2");
        }
    }
    free(copy1);

    int stats_none_low[] = {10, 11, 12};
    int out_len2 = -1;
    int *copy2 = copy_under_limit(stats_none_low, 3, 5, &out_len2);

    CHECK_NULL(copy2, "copy_under_limit({10,11,12}, 3, 5, ...) -- none under limit");
    CHECK_INT(out_len2, 0, "out_len2 when nothing matches");
}

/* ===== runner ===== */

int main(void) {
    printf("Running checks for first_stat_over / copy_under_limit\n\n");

    test_pointers_run_basic();
    test_pointers_run_copy();
    test_pointers_submit_first();
    test_pointers_submit_copy_edges();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}