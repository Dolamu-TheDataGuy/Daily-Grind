#include <stdio.h>
#include <string.h>

void compact_spaces(char *s);

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

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

void test_compact_spaces_run_basic(void)
{
    printf("  test_compact_spaces_run_basic\n");

    char buf1[] = "  Launch   the   rockets!  ";
    compact_spaces(buf1);
    CHECK_STR(buf1, "Launch the rockets!", "buf1 mismatch");

    char buf2[] = "NoExtraSpaces";
    compact_spaces(buf2);
    CHECK_STR(buf2, "NoExtraSpaces", "buf2 mismatch");

    char buf3[] = "";
    compact_spaces(buf3);
    CHECK_STR(buf3, "", "buf3 mismatch");
}

void test_compact_spaces_submit_edges(void)
{
    printf("  test_compact_spaces_submit_edges\n");

    char buf1[] = "     ";
    compact_spaces(buf1);
    CHECK_STR(buf1, "", "all spaces -> empty");

    char buf2[] = "  a   b   c  ";
    compact_spaces(buf2);
    CHECK_STR(buf2, "a b c", "a b c mismatch");
}

void test_compact_spaces_submit_happy_end(void)
{
    printf("  test_compact_spaces_submit_happy_end\n");

    char buf[] = "Hello,   world!   This  is   C. ";
    compact_spaces(buf);
    CHECK_STR(buf, "Hello, world! This is C.", "long sentence mismatch");
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for compact_spaces\n\n");

    test_compact_spaces_run_basic();
    test_compact_spaces_submit_edges();
    test_compact_spaces_submit_happy_end();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}