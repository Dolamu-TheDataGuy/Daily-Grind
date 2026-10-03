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

/* frees what split_words heap-allocated, mirroring what the real test
   harness does on our behalf -- needed here since we're not using munit */
static void free_words(char **words, int count)
{
    if (!words)
        return;
    for (int i = 0; i < count; i++)
    {
        free(words[i]);
    }
    free(words);
}

/* ===== the five cases from the pasted main.c, reproduced in plain C ===== */

void test_split_words_simple(void)
{
    printf("  test_split_words_simple\n");

    int count = 0;
    char **words = split_words("hello world from c", &count);

    CHECK_INT(count, 4, "split_words(\"hello world from c\") -- count");
    if (words && count == 4)
    {
        CHECK_STR(words[0], "hello", "word 0");
        CHECK_STR(words[1], "world", "word 1");
        CHECK_STR(words[2], "from", "word 2");
        CHECK_STR(words[3], "c", "word 3");
    }

    free_words(words, count);
}

void test_split_words_with_spaces(void)
{
    printf("  test_split_words_with_spaces\n");

    int count = 0;
    const char *input = "  multiple   spaces  here ";
    char **words = split_words(input, &count);

    CHECK_INT(count, 3, "split_words(multi-space input) -- count");
    if (words && count == 3)
    {
        CHECK_STR(words[0], "multiple", "word 0");
        CHECK_STR(words[1], "spaces", "word 1");
        CHECK_STR(words[2], "here", "word 2");
    }

    free_words(words, count);
}

void test_split_words_empty(void)
{
    printf("  test_split_words_empty\n");

    int count = 123; /* deliberately garbage, to prove it gets overwritten */
    const char *input = "   ";
    char **words = split_words(input, &count);

    CHECK_INT(count, 0, "split_words(all-space input) -- count");
    CHECK_NULL(words, "split_words(all-space input) -- words");

    free_words(words, count);
}

void test_split_words_single(void)
{
    printf("  test_split_words_single\n");

    int count = 0;
    const char *input = "Boot.dev";
    char **words = split_words(input, &count);

    CHECK_INT(count, 1, "split_words(\"Boot.dev\") -- count");
    if (words && count == 1)
    {
        CHECK_STR(words[0], "Boot.dev", "single word");
    }

    free_words(words, count);
}

void test_split_words_tricky(void)
{
    printf("  test_split_words_tricky\n");

    int count = 0;
    const char *input = " split  this tricky   string   into words ";
    char **words = split_words(input, &count);

    CHECK_INT(count, 6, "split_words(tricky spacing) -- count");
    if (words && count == 6)
    {
        CHECK_STR(words[0], "split", "w0");
        CHECK_STR(words[1], "this", "w1");
        CHECK_STR(words[2], "tricky", "w2");
        CHECK_STR(words[3], "string", "w3");
        CHECK_STR(words[4], "into", "w4");
        CHECK_STR(words[5], "words", "w5");
    }

    free_words(words, count);
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for split_words\n\n");

    test_split_words_simple();
    test_split_words_with_spaces();
    test_split_words_empty();
    test_split_words_single();
    test_split_words_tricky();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}