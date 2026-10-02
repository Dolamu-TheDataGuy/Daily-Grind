#include "munit.h"
#include <stddef.h>

void move_players(int *positions, int length, int delta);
int *find_player(int *positions, int length, int target);

munit_case(RUN, test_move_and_find_basic, {
    int positions[5] = {10, 20, 30, 40, 50};
    int length = 5;

    move_players(positions, length, 3);

    int expected_after[5] = {13, 23, 33, 43, 53};
    for (int i = 0; i < length; i++)
    {
        munit_assert_int_equal(
            positions[i],
            expected_after[i],
            "Input positions: {10, 20, 30, 40, 50}, delta: 3");
    }

    int *found = find_player(positions, length, 33);
    munit_assert_ptr_equal(
        found,
        &positions[2],
        "Input positions: {13, 23, 33, 43, 53}, target: 33");
});

munit_case(RUN, test_move_negative_delta, {
    int positions[4] = {5, 15, 25, 35};
    int length = 4;

    move_players(positions, length, -5);

    int expected_after[4] = {0, 10, 20, 30};
    for (int i = 0; i < length; i++)
    {
        munit_assert_int_equal(
            positions[i],
            expected_after[i],
            "Input positions: {5, 15, 25, 35}, delta: -5");
    }
});

munit_case(SUBMIT, test_find_not_found, {
    int positions[3] = {100, 200, 300};
    int length = 3;

    int *found = find_player(positions, length, 999);
    munit_assert_null(
        found,
        "Input positions: {100, 200, 300}, target: 999 (should not be found)");
});

munit_case(SUBMIT, test_move_and_find_multiple, {
    int positions[7] = {0, 10, 20, 30, 40, 50, 60};
    int length = 7;

    move_players(positions, length, 7);

    int expected_after[7] = {7, 17, 27, 37, 47, 57, 67};
    for (int i = 0; i < length; i++)
    {
        munit_assert_int_equal(
            positions[i],
            expected_after[i],
            "Input positions: {0, 10, 20, 30, 40, 50, 60}, delta: 7");
    }

    /* Check first match is returned when there are multiple equal values */
    positions[1] = 99;
    positions[4] = 99;

    int *found = find_player(positions, length, 99);
    munit_assert_ptr_equal(
        found,
        &positions[1],
        "Input positions: {7, 99, 27, 37, 99, 57, 67}, target: 99 (should return first match)");
});

int main()
{
    MunitTest tests[] = {
        munit_test("/run/move_and_find_basic", test_move_and_find_basic),
        munit_test("/run/move_negative_delta", test_move_negative_delta),
        munit_test("/submit/find_not_found", test_find_not_found),
        munit_test("/submit/move_and_find_multi", test_move_and_find_multiple),
        munit_null_test,
    };

    MunitSuite suite = munit_suite("player_positions_pointers", tests);
    return munit_suite_main(&suite, NULL, 0, NULL);
}
