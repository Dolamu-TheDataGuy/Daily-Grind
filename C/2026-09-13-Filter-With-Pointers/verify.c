#include <stdio.h>
#include <string.h>

int filter_even(int *numbers, int length);
void squeeze_spaces(char *text);

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

#define CHECK_STR(actual, expected, msg)                              \
    do                                                                \
    {                                                                 \
        const char *check_str_actual_ = (actual);                     \
        tests_run++;                                                  \
        if (check_str_actual_ != NULL &&                              \
            strcmp(check_str_actual_, (expected)) == 0)               \
        {                                                             \
            tests_passed++;                                           \
        }                                                             \
        else                                                          \
        {                                                             \
            printf("    FAIL: %s -- expected \"%s\", got \"%s\"\n",   \
                   msg, expected,                                     \
                   check_str_actual_ ? check_str_actual_ : "(null)"); \
        }                                                             \
    } while (0)

/* ===== the six cases from the pasted main.c, reproduced in plain C ===== */

void test_filter_even_run_basic(void)
{
    printf("  test_filter_even_run_basic\n");

    int nums[5] = {1, 2, 3, 4, 5};
    int result = filter_even(nums, 5);

    CHECK_INT(result, 2, "filter_even({1,2,3,4,5}, 5) -- even count");
    CHECK_INT(nums[0], 2, "nums[0] after filter");
    CHECK_INT(nums[1], 4, "nums[1] after filter");
}

void test_filter_even_run_all_even(void)
{
    printf("  test_filter_even_run_all_even\n");

    int nums[3] = {2, 4, 6};
    int result = filter_even(nums, 3);

    CHECK_INT(result, 3, "filter_even({2,4,6}, 3) -- even count");
    CHECK_INT(nums[0], 2, "order preserved -- nums[0]");
    CHECK_INT(nums[1], 4, "order preserved -- nums[1]");
    CHECK_INT(nums[2], 6, "order preserved -- nums[2]");
}

void test_squeeze_spaces_run_simple(void)
{
    printf("  test_squeeze_spaces_run_simple\n");

    char text1[] = "hello world";
    squeeze_spaces(text1);
    CHECK_STR(text1, "helloworld", "squeeze_spaces(\"hello world\")");

    char text2[] = "no  spaces";
    squeeze_spaces(text2);
    CHECK_STR(text2, "nospaces", "squeeze_spaces(\"no  spaces\") -- double space");
}

void test_filter_even_submit_edges(void)
{
    printf("  test_filter_even_submit_edges\n");

    int nums1[4] = {1, 3, 5, 7};
    int count1 = filter_even(nums1, 4);
    CHECK_INT(count1, 0, "filter_even({1,3,5,7}, 4) -- no evens");

    int nums2[1] = {42};
    int count2 = filter_even(nums2, 0);
    CHECK_INT(count2, 0, "filter_even(nums2, 0) -- zero length");
}

void test_filter_even_submit_big_happy(void)
{
    printf("  test_filter_even_submit_big_happy\n");

    int nums[7] = {10, 9, 8, 7, 6, 5, 4};
    int result = filter_even(nums, 7);

    CHECK_INT(result, 4, "filter_even({10,9,8,7,6,5,4}, 7) -- even count");
    CHECK_INT(nums[0], 10, "nums[0]");
    CHECK_INT(nums[1], 8, "nums[1]");
    CHECK_INT(nums[2], 6, "nums[2]");
    CHECK_INT(nums[3], 4, "nums[3]");
}

void test_squeeze_spaces_submit(void)
{
    printf("  test_squeeze_spaces_submit\n");

    char text1[] = "   leading and  inner  spaces   ";
    squeeze_spaces(text1);
    CHECK_STR(text1, "leadingandinnerspaces",
              "squeeze_spaces(leading/trailing/multi-inner spaces)");

    char text2[] = "";
    squeeze_spaces(text2);
    CHECK_STR(text2, "", "squeeze_spaces(\"\") -- empty string stays empty");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for filter_even / squeeze_spaces\n\n");

    test_filter_even_run_basic();
    test_filter_even_run_all_even();
    test_squeeze_spaces_run_simple();
    test_filter_even_submit_edges();
    test_filter_even_submit_big_happy();
    test_squeeze_spaces_submit();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}