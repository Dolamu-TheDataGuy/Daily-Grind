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

/* ===== the five cases from the pasted main.c, reproduced in plain C ===== */

void test_inventory_basic(void)
{
    printf("  test_inventory_basic\n");

    const char *names[] = {"Potion", "Elixir"};
    int quantities[] = {3, 1};

    Inventory *inv = create_inventory(names, quantities, 2);

    CHECK_PTR_NOT_NULL(inv, "create_inventory([Potion,Elixir], [3,1], 2) -- result");
    if (inv)
    {
        CHECK_INT(inv->size, 2, "inv->size");
        CHECK_STR(inv->items[0].name, "Potion", "items[0].name");
        CHECK_INT(inv->items[0].quantity, 3, "items[0].quantity");
        CHECK_STR(inv->items[1].name, "Elixir", "items[1].name");
        CHECK_INT(inv->items[1].quantity, 1, "items[1].quantity");
    }

    free_inventory(inv);
}

void test_inventory_independent_copy(void)
{
    printf("  test_inventory_independent_copy\n");

    const char *names[] = {"Sword"};
    int quantities[] = {2};

    Inventory *inv = create_inventory(names, quantities, 1);
    CHECK_PTR_NOT_NULL(inv, "create_inventory([Sword], [2], 1) -- result");

    /* mutate the caller's own array AFTER creation -- stored data must
       not be affected, proving the function copied bytes, not pointers */
    names[0] = "Axe";

    if (inv)
    {
        CHECK_STR(inv->items[0].name, "Sword", "items[0].name must stay \"Sword\" after caller mutates names[]");
        CHECK_INT(inv->items[0].quantity, 2, "items[0].quantity");
    }

    free_inventory(inv);
}

void test_inventory_zero_count(void)
{
    printf("  test_inventory_zero_count\n");

    Inventory *inv = create_inventory(NULL, NULL, 0);
    CHECK_NULL(inv, "create_inventory(NULL, NULL, 0) -- must be NULL");
}

void test_inventory_truncate_name(void)
{
    printf("  test_inventory_truncate_name\n");

    const char *names[] = {"VeryVeryVeryLongItemNameThatShouldBeCutOff"};
    int quantities[] = {5};

    Inventory *inv = create_inventory(names, quantities, 1);
    CHECK_PTR_NOT_NULL(inv, "create_inventory(long name, [5], 1) -- result");

    char expected[32];
    const char *src = names[0];
    int i = 0;
    while (src[i] != '\0' && i < 31)
    {
        expected[i] = src[i];
        i++;
    }
    expected[i] = '\0';

    if (inv)
    {
        CHECK_STR(inv->items[0].name, expected, "items[0].name -- truncated to 31 chars + NUL");
        CHECK_INT((int)strlen(inv->items[0].name), 31, "items[0].name -- truncated length must be exactly 31");
        CHECK_INT(inv->items[0].quantity, 5, "items[0].quantity");
    }

    free_inventory(inv);
}

void test_inventory_three_items(void)
{
    printf("  test_inventory_three_items\n");

    const char *names[] = {"Arrow", "Shield", "Herb"};
    int quantities[] = {10, 1, 7};

    Inventory *inv = create_inventory(names, quantities, 3);

    CHECK_PTR_NOT_NULL(inv, "create_inventory([Arrow,Shield,Herb], [10,1,7], 3) -- result");
    if (inv)
    {
        CHECK_INT(inv->size, 3, "inv->size");
        CHECK_STR(inv->items[0].name, "Arrow", "items[0].name");
        CHECK_INT(inv->items[0].quantity, 10, "items[0].quantity");
        CHECK_STR(inv->items[1].name, "Shield", "items[1].name");
        CHECK_INT(inv->items[1].quantity, 1, "items[1].quantity");
        CHECK_STR(inv->items[2].name, "Herb", "items[2].name");
        CHECK_INT(inv->items[2].quantity, 7, "items[2].quantity");
    }

    free_inventory(inv);
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for create_inventory / free_inventory\n\n");

    test_inventory_basic();
    test_inventory_independent_copy();
    test_inventory_zero_count();
    test_inventory_truncate_name();
    test_inventory_three_items();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}