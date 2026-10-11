#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exercise.h"

/* ===== tiny plain-C test helpers (no external library) ===== */

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_PTR_NULL(actual, msg)                                       \
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

/* helper: build a new heap-allocated Song node, mirroring the pasted
 * munit test file's new_song() helper */
static Song *new_song(const char *title)
{
    Song *s = malloc(sizeof(Song));
    if (!s)
        return NULL;
    s->title = title;
    s->next = NULL;
    return s;
}

/* helper: walk the list from head and compare titles against an
 * expected array, then confirm the list ends in NULL -- mirrors the
 * pasted munit test file's assert_playlist_titles() helper */
static void check_playlist_titles(Song *head, const char **titles,
                                  int count, const char *msg)
{
    Song *cur = head;
    int i;

    for (i = 0; i < count; i++)
    {
        tests_run++;
        if (cur == NULL)
        {
            printf("    FAIL: %s -- list ended early at index %d (expected \"%s\")\n",
                   msg, i, titles[i]);
            return;
        }
        if (strcmp(cur->title, titles[i]) == 0)
        {
            tests_passed++;
        }
        else
        {
            printf("    FAIL: %s -- index %d expected \"%s\", got \"%s\"\n",
                   msg, i, titles[i], cur->title);
        }
        cur = cur->next;
    }

    CHECK_PTR_NULL(cur, msg);
}

/* ===== the four cases from the pasted main.c, reproduced in plain C ===== */

void test_playlist_run_basic_push_pop(void)
{
    printf("  test_playlist_run_basic_push_pop\n");

    Song *head = NULL;

    Song *a = new_song("A");
    Song *b = new_song("B");
    Song *c = new_song("C");

    playlist_push_back(&head, a);  /* [A] */
    playlist_push_back(&head, b);  /* [A, B] */
    playlist_push_front(&head, c); /* [C, A, B] */

    const char *expected1[] = {"C", "A", "B"};
    check_playlist_titles(head, expected1, 3, "After pushes: expected [C, A, B]");

    Song *first = playlist_pop_front(&head); /* remove C */
    CHECK_PTR_NOT_NULL(first, "pop_front should return node");
    if (first)
    {
        CHECK_STR(first->title, "C", "Popped title should be C");
        CHECK_PTR_NULL(first->next, "Popped node next should be NULL");
    }

    const char *expected2[] = {"A", "B"};
    check_playlist_titles(head, expected2, 2, "After pop: expected [A, B]");

    free(first);
    playlist_clear(&head);
}

void test_playlist_run_find_extract(void)
{
    printf("  test_playlist_run_find_extract\n");

    Song *head = NULL;

    Song *intro = new_song("Intro");
    Song *boss = new_song("Boss");
    Song *end = new_song("Credits");

    playlist_push_back(&head, intro); /* [Intro] */
    playlist_push_back(&head, boss);  /* [Intro, Boss] */
    playlist_push_back(&head, end);   /* [Intro, Boss, Credits] */

    Song *found = playlist_find(head, "Boss");
    CHECK_PTR_NOT_NULL(found, "Should find 'Boss'");
    if (found)
    {
        CHECK_STR(found->title, "Boss", "Found title should be Boss");
    }

    Song *removed = playlist_extract(&head, "Boss");
    CHECK_PTR_NOT_NULL(removed, "Extract should return removed node");
    if (removed)
    {
        CHECK_STR(removed->title, "Boss", "Removed title should be Boss");
        CHECK_PTR_NULL(removed->next, "Removed node next should be NULL");
    }

    const char *expected[] = {"Intro", "Credits"};
    check_playlist_titles(head, expected, 2, "After extract: expected [Intro, Credits]");

    free(removed);
    playlist_clear(&head);
}

void test_playlist_submit_edges_and_clear(void)
{
    printf("  test_playlist_submit_edges_and_clear\n");

    Song *head = NULL;

    /* Pop and extract on empty list */
    Song *p = playlist_pop_front(&head);
    CHECK_PTR_NULL(p, "Pop on empty list should return NULL");

    Song *e = playlist_extract(&head, "Anything");
    CHECK_PTR_NULL(e, "Extract on empty list should return NULL");

    /* Build a longer list */
    const char *titles[] = {"Track1", "Track2", "Track3", "Track4"};
    int i;
    for (i = 0; i < 4; i++)
    {
        Song *s = new_song(titles[i]);
        playlist_push_back(&head, s);
    }

    const char *expected1[] = {"Track1", "Track2", "Track3", "Track4"};
    check_playlist_titles(head, expected1, 4, "Initial playlist order incorrect");

    /* Extract middle element */
    Song *mid = playlist_extract(&head, "Track3");
    CHECK_PTR_NOT_NULL(mid, "Should extract Track3");
    if (mid)
    {
        CHECK_STR(mid->title, "Track3", "Extracted title should be Track3");
        CHECK_PTR_NULL(mid->next, "Extracted node next should be NULL");
    }

    const char *expected2[] = {"Track1", "Track2", "Track4"};
    check_playlist_titles(head, expected2, 3, "After extracting Track3");

    /* Extract head element */
    Song *head_removed = playlist_extract(&head, "Track1");
    CHECK_PTR_NOT_NULL(head_removed, "Should extract Track1 (head)");
    if (head_removed)
    {
        CHECK_STR(head_removed->title, "Track1", "Extracted head title should be Track1");
    }

    const char *expected3[] = {"Track2", "Track4"};
    check_playlist_titles(head, expected3, 2, "After extracting head Track1");

    free(mid);
    free(head_removed);

    /* Finally clear all */
    playlist_clear(&head);
    CHECK_PTR_NULL(head, "After clear, head should be NULL");
}

void test_playlist_submit_big_happy_path(void)
{
    printf("  test_playlist_submit_big_happy_path\n");

    Song *head = NULL;

    const char *inputs[] = {
        "SongA", "SongB", "SongC", "SongD", "SongE",
        "SongF", "SongG", "SongH", "SongI", "SongJ"};

    int i;
    for (i = 0; i < 5; i++)
    {
        Song *s = new_song(inputs[i]);
        playlist_push_back(&head, s);
    }

    for (i = 5; i < 10; i++)
    {
        Song *s = new_song(inputs[i]);
        playlist_push_front(&head, s);
    }

    /* After these operations, the list should be:
     * [SongJ, SongI, SongH, SongG, SongF, SongA, SongB, SongC, SongD, SongE] */
    const char *expected1[] = {
        "SongJ", "SongI", "SongH", "SongG", "SongF",
        "SongA", "SongB", "SongC", "SongD", "SongE"};
    check_playlist_titles(head, expected1, 10,
                          "push_back A-E then push_front F-J -- expected J I H G F A B C D E");

    /* Pop a few from the front */
    Song *x1 = playlist_pop_front(&head);
    Song *x2 = playlist_pop_front(&head);
    Song *x3 = playlist_pop_front(&head);

    if (x1)
        CHECK_STR(x1->title, "SongJ", "First pop should be SongJ");
    if (x2)
        CHECK_STR(x2->title, "SongI", "Second pop should be SongI");
    if (x3)
        CHECK_STR(x3->title, "SongH", "Third pop should be SongH");

    const char *expected2[] = {
        "SongG", "SongF", "SongA", "SongB", "SongC", "SongD", "SongE"};
    check_playlist_titles(head, expected2, 7,
                          "After popping 3 songs, expected [G, F, A, B, C, D, E]");

    free(x1);
    free(x2);
    free(x3);

    playlist_clear(&head);
}

/* ===== runner ===== */

int main(void)
{
    printf("Running checks for playlist_push_front / push_back / pop_front / find / extract / clear\n\n");

    test_playlist_run_basic_push_pop();
    test_playlist_run_find_extract();
    test_playlist_submit_edges_and_clear();
    test_playlist_submit_big_happy_path();

    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}