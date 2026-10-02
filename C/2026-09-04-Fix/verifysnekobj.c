#include <stdio.h>
#include <string.h>
#include "exercise.h"

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_INT(actual, expected, msg)                      \
    do                                                        \
    {                                                         \
        tests_run++;                                          \
        if ((actual) == (expected))                           \
        {                                                     \
            tests_passed++;                                   \
        }                                                     \
        else                                                  \
        {                                                     \
            printf("    FAIL: %s -- expected %ld, got %ld\n", \
                   msg, (long)(expected), (long)(actual));    \
        }                                                     \
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

/* ===== the five cases from the boot.dev main.c, reproduced in plain C ===== */

void test_snek_add_ints(void)
{
    printf("  test_snek_add_ints\n");

    snek_obj_t *a = snek_new_int(10);
    snek_obj_t *b = snek_new_int(5);

    snek_obj_t *result = snek_add(a, b);

    CHECK_PTR_NOT_NULL(result, "snek_add(10, 5) -- result");
    if (result)
    {
        CHECK_INT(result->type, SNEK_INT, "snek_add(10, 5) -- result type");
        CHECK_INT(result->as.int_value, 15, "snek_add(10, 5) -- result value");
    }
    CHECK_INT(a->as.int_value, 10, "snek_add(10, 5) -- left operand must stay 10");
    CHECK_INT(b->as.int_value, 5, "snek_add(10, 5) -- right operand must stay 5");

    snek_free(a);
    snek_free(b);
    snek_free(result);
}

void test_snek_add_strings(void)
{
    printf("  test_snek_add_strings\n");

    snek_obj_t *hello = snek_new_string("Hello, ");
    snek_obj_t *world = snek_new_string("world");

    snek_obj_t *joined = snek_add(hello, world);

    CHECK_PTR_NOT_NULL(joined, "snek_add(\"Hello, \", \"world\") -- result");
    if (joined)
    {
        CHECK_INT(joined->type, SNEK_STRING, "snek_add(\"Hello, \", \"world\") -- result type");
        CHECK_STR(joined->as.str.data, "Hello, world", "snek_add(\"Hello, \", \"world\") -- data");
        CHECK_INT(joined->as.str.length, 12, "snek_add(\"Hello, \", \"world\") -- length");
    }
    CHECK_STR(hello->as.str.data, "Hello, ", "left string operand must stay \"Hello, \"");
    CHECK_STR(world->as.str.data, "world", "right string operand must stay \"world\"");

    snek_free(hello);
    snek_free(world);
    snek_free(joined);
}

void test_snek_add_empty_strings(void)
{
    printf("  test_snek_add_empty_strings\n");

    snek_obj_t *empty = snek_new_string("");
    snek_obj_t *msg = snek_new_string("Snek!");

    snek_obj_t *r1 = snek_add(empty, msg);
    snek_obj_t *r2 = snek_add(msg, empty);

    CHECK_PTR_NOT_NULL(r1, "snek_add(\"\", \"Snek!\") -- result");
    CHECK_PTR_NOT_NULL(r2, "snek_add(\"Snek!\", \"\") -- result");

    if (r1)
    {
        CHECK_STR(r1->as.str.data, "Snek!", "snek_add(\"\", \"Snek!\") -- data");
        CHECK_INT(r1->as.str.length, 5, "snek_add(\"\", \"Snek!\") -- length");
    }
    if (r2)
    {
        CHECK_STR(r2->as.str.data, "Snek!", "snek_add(\"Snek!\", \"\") -- data");
        CHECK_INT(r2->as.str.length, 5, "snek_add(\"Snek!\", \"\") -- length");
    }

    snek_free(empty);
    snek_free(msg);
    snek_free(r1);
    snek_free(r2);
}

void test_snek_add_mismatched_types(void)
{
    printf("  test_snek_add_mismatched_types\n");

    snek_obj_t *num = snek_new_int(42);
    snek_obj_t *txt = snek_new_string("forty-two");

    snek_obj_t *r1 = snek_add(num, txt);
    snek_obj_t *r2 = snek_add(txt, num);

    CHECK_NULL(r1, "snek_add(INT(42), STRING(\"forty-two\")) -- must be NULL");
    CHECK_NULL(r2, "snek_add(STRING(\"forty-two\"), INT(42)) -- must be NULL");

    CHECK_INT(num->as.int_value, 42, "INT operand must not be modified by failed addition");
    CHECK_STR(txt->as.str.data, "forty-two", "STRING operand must not be modified by failed addition");

    snek_free(num);
    snek_free(txt);
}

void test_snek_add_long_strings(void)
{
    printf("  test_snek_add_long_strings\n");

    const char *left_text = "Snek objects on the ";
    const char *right_text = "stack and heap!";
    const char *expected = "Snek objects on the stack and heap!";

    snek_obj_t *left = snek_new_string(left_text);
    snek_obj_t *right = snek_new_string(right_text);

    snek_obj_t *joined = snek_add(left, right);

    CHECK_PTR_NOT_NULL(joined, "snek_add(long strings) -- result");
    if (joined)
    {
        CHECK_STR(joined->as.str.data, expected, "snek_add(long strings) -- data");
        CHECK_INT(joined->as.str.length, (long)strlen(expected), "snek_add(long strings) -- length");
    }

    snek_free(left);
    snek_free(right);
    snek_free(joined);
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for snek_add\n\n");

    test_snek_add_ints();
    test_snek_add_strings();
    test_snek_add_empty_strings();
    test_snek_add_mismatched_types();
    test_snek_add_long_strings();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}