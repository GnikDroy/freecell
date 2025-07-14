#include <assert.h>
#include <stdio.h>

#include "freecell.h"

void print_test_result(const char* test_name, bool passed) {
    printf("%s: %s\n", test_name, passed ? "PASS" : "FAIL");
}

void test_freecell_push_and_pop_cascade(void) {
    Cascade c = { .size = 0 };
    Card card = 5;

    uint8_t size_after_push = cascade_push(&c, card);
    assert(size_after_push == 1);
    assert(c.cards[0] == card);

    Card popped = cascade_pop(&c);
    assert(popped == card);
    assert(c.size == 0);

    popped = cascade_pop(&c);
    assert(popped == NONE);

    print_test_result("test_freecell_push_and_pop_cascade", true);
}

void test_freecell_init_distribution(void) {
    Freecell game = freecell_init();

    bool pass = true;
    for (int i = 0; i < 4; i++) {
        if (game.cascade[i].size != 7) {
            pass = false;
            break;
        }
    }
    for (int i = 4; i < 8; i++) {
        if (game.cascade[i].size != 6) {
            pass = false;
            break;
        }
    }
    print_test_result("test_freecell_init_distribution", pass);
    assert(pass);
}

void test_suits_differ_by_color(void) {
    assert(suits_differ_by_color(SPADES, HEARTS));
    assert(suits_differ_by_color(CLUBS, DIAMONDS));
    assert(suits_differ_by_color(HEARTS, CLUBS));
    assert(suits_differ_by_color(DIAMONDS, SPADES));

    assert(!suits_differ_by_color(SPADES, CLUBS));
    assert(!suits_differ_by_color(HEARTS, DIAMONDS));
    assert(!suits_differ_by_color(SPADES, SPADES));
    print_test_result("test_suits_differ_by_color", true);
}

void test_freecell_move_to_foundation(void) {
    Freecell game = { 0 };
    Card ace_spades = ACE_SPADES;
    Card two_spades = TWO_SPADES;

    MoveResult res = freecell_validate_to_foundation(&game, ace_spades);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_foundation(&game, ace_spades);
    assert(game.foundation[SPADES] == ace_spades);

    res = freecell_validate_to_foundation(&game, two_spades);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_foundation(&game, two_spades);
    assert(game.foundation[SPADES] == two_spades);

    res = freecell_validate_to_foundation(&game, two_spades);
    assert(res == MOVE_ERROR);

    print_test_result("test_freecell_move_to_foundation", true);
}

void test_freecell_move_to_reserve(void) {
    Freecell game = { 0 };
    Card card = ACE_SPADES;

    MoveResult res = freecell_validate_to_reserve(&game, card, RESERVE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_reserve(&game, card, RESERVE_1);
    assert(game.reserve[0] == card);

    res = freecell_validate_to_reserve(&game, TWO_SPADES, RESERVE_1);
    assert(res == MOVE_ERROR);

    res = freecell_validate_to_reserve(&game, NONE, RESERVE_2);
    assert(res == MOVE_ERROR);

    print_test_result("test_freecell_move_to_reserve", true);
}

void test_freecell_move_to_cascade_single(void) {
    Freecell game = { 0 };
    Cascade* c = &game.cascade[0];

    Card card = TWO_SPADES;
    MoveResult res = freecell_validate_to_cascade_single(&game, card, CASCADE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_cascade_single(&game, card, CASCADE_1);
    assert(c->size == 1);
    assert(c->cards[0] == card);

    Card card2 = ACE_SPADES;
    res = freecell_validate_to_cascade_single(&game, card2, CASCADE_1);
    assert(res == MOVE_ERROR_WRONG_SUIT);

    Card card3 = THREE_DIAMONDS;
    res = freecell_validate_to_cascade_single(&game, card3, CASCADE_1);
    assert(res == MOVE_ERROR_WRONG_RANK);

    Card card4 = ACE_DIAMONDS;
    res = freecell_validate_to_cascade_single(&game, card4, CASCADE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_cascade_single(&game, card4, CASCADE_1);
    assert(c->size == 2);
    assert(c->cards[1] == card4);

    print_test_result("test_freecell_move_to_cascade_single", true);
}

void test_freecell_move_to_foundation_wrong_rank(void) {
    Freecell game = { 0 };
    Card ace_spades = ACE_SPADES;
    Card three_spades = THREE_SPADES;

    MoveResult res = freecell_validate_to_foundation(&game, ace_spades);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_foundation(&game, ace_spades);

    res = freecell_validate_to_foundation(&game, three_spades);
    assert(res == MOVE_ERROR);

    print_test_result("test_freecell_move_to_foundation_wrong_rank", true);
}

void test_freecell_move_to_reserve_full_slot(void) {
    Freecell game = { 0 };
    Card card1 = ACE_SPADES;
    Card card2 = TWO_HEARTS;

    MoveResult res = freecell_validate_to_reserve(&game, card1, RESERVE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_reserve(&game, card1, RESERVE_1);

    res = freecell_validate_to_reserve(&game, card2, RESERVE_1);
    assert(res == MOVE_ERROR);

    print_test_result("test_freecell_move_to_reserve_full_slot", true);
}

void test_freecell_push_pop_multiple_cascade(void) {
    Cascade c = { .size = 0 };
    Card cards[] = { ACE_HEARTS, TWO_SPADES, THREE_DIAMONDS };

    for (int i = 0; i < 3; i++) {
        uint8_t size_after_push = cascade_push(&c, cards[i]);
        assert(size_after_push == i + 1);
    }

    for (int i = 2; i >= 0; i--) {
        Card popped = cascade_pop(&c);
        assert(popped == cards[i]);
    }

    assert(c.size == 0);

    print_test_result("test_freecell_push_pop_multiple_cascade", true);
}

void test_suits_differ_by_color_same_suit(void) {

    assert(!suits_differ_by_color(SPADES, SPADES));
    assert(!suits_differ_by_color(HEARTS, HEARTS));
    assert(!suits_differ_by_color(DIAMONDS, DIAMONDS));
    assert(!suits_differ_by_color(CLUBS, CLUBS));

    print_test_result("test_suits_differ_by_color_same_suit", true);
}

void test_freecell_move_to_cascade_invalid_rank_gap(void) {
    Freecell game = { 0 };

    Card card = FOUR_SPADES;
    MoveResult res = freecell_validate_to_cascade_single(&game, card, CASCADE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_cascade_single(&game, card, CASCADE_1);

    Card card2 = TWO_HEARTS;

    res = freecell_validate_to_cascade_single(&game, card2, CASCADE_1);
    assert(res == MOVE_ERROR_WRONG_RANK);

    print_test_result("test_freecell_move_to_cascade_invalid_rank_gap", true);
}

void test_freecell_move_reserve_to_cascade(void) {
    Freecell game = { 0 };

    Card card = THREE_DIAMONDS;
    MoveResult res = freecell_validate_to_reserve(&game, card, RESERVE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_reserve(&game, card, RESERVE_1);

    res = freecell_validate_to_cascade_single(&game, card, CASCADE_1);
    assert(res == MOVE_SUCCESS);
    freecell_move_to_cascade_single(&game, card, CASCADE_1);

    game.reserve[0] = NONE;

    print_test_result("test_freecell_move_reserve_to_cascade", true);
}

void test_freecell_move_to_foundation_invalid_start(void) {
    Freecell game = { 0 };
    Card two_spades = TWO_SPADES;

    MoveResult res = freecell_validate_to_foundation(&game, two_spades);
    assert(res == MOVE_ERROR);

    print_test_result("test_freecell_move_to_foundation_invalid_start", true);
}

void test_freecell_valid_multi_card_cascade_move(void) {
    Freecell game = { 0 };

    Card cards[] = { KING_SPADES, QUEEN_HEARTS, JACK_CLUBS };
    Cascade* from = &game.cascade[0];
    from->size = 3;
    for (int i = 0; i < 3; ++i)
        from->cards[i] = cards[i];

    Cascade* to = &game.cascade[1];
    to->size = 0;

    Move move = {
        .from = CASCADE_1,
        .to = CASCADE_2,
        .size = 3,
    };

    MoveResult result = freecell_validate_to_cascade(&game, move);
    assert(result == MOVE_SUCCESS);
    freecell_move_to_cascade(&game, move);

    assert(to->size == 3);
    assert(memcmp(to->cards, cards, sizeof(cards)) == 0);

    print_test_result("test_freecell_valid_multi_card_cascade_move", true);
}

void test_freecell_invalid_multi_card_wrong_color(void) {
    Freecell game = { 0 };

    Card cards[] = { KING_SPADES, QUEEN_CLUBS, JACK_SPADES };
    Cascade* from = &game.cascade[0];
    from->size = 3;
    for (int i = 0; i < 3; ++i)
        from->cards[i] = cards[i];

    Move move = {
        .from = CASCADE_1,
        .to = CASCADE_2,
        .size = 3,
    };

    MoveResult result = freecell_validate_to_cascade(&game, move);
    assert(result == MOVE_ERROR_WRONG_SUIT);

    print_test_result("test_freecell_invalid_multi_card_wrong_color", true);
}

void test_freecell_invalid_move_exceed_max_moves(void) {
    Freecell game = { 0 };

    for (int i = 0; i < 4; i++)
        game.reserve[i] = ACE_SPADES;
    for (int i = 0; i < 8; i++)
        game.cascade[i].size = 1;

    Cascade* from = &game.cascade[0];
    from->size = 3;
    from->cards[0] = KING_SPADES;
    from->cards[1] = QUEEN_HEARTS;
    from->cards[2] = JACK_CLUBS;

    Move move = {
        .from = CASCADE_1,
        .to = CASCADE_2,
        .size = 3,
    };

    MoveResult result = freecell_validate_to_cascade(&game, move);
    assert(result == MOVE_ERROR_INSUFFICIENT_FREECELLS);

    print_test_result("test_freecell_invalid_move_exceed_max_moves", true);
}

void test_freecell_reserve_to_cascade_wrong_suit(void) {
    Freecell game = { 0 };

    game.reserve[0] = THREE_SPADES;

    Cascade* c = &game.cascade[0];
    c->size = 1;
    c->cards[0] = FOUR_CLUBS;

    MoveResult result = freecell_validate_to_cascade_single(&game, THREE_SPADES, CASCADE_1);
    assert(result == MOVE_ERROR_WRONG_SUIT);

    print_test_result("test_freecell_reserve_to_cascade_wrong_suit", true);
}

void test_freecell_game_over_check(void) {
    Freecell game = { 0 };

    game.foundation[SPADES] = KING_SPADES;
    game.foundation[HEARTS] = KING_HEARTS;
    game.foundation[DIAMONDS] = KING_DIAMONDS;
    game.foundation[CLUBS] = KING_CLUBS;

    assert(freecell_game_over(&game) == true);

    print_test_result("test_freecell_game_over_check", true);
}

void test_freecell_game_not_over_due_to_reserve(void) {
    Freecell game = { 0 };

    game.foundation[SPADES] = KING_SPADES;
    game.foundation[HEARTS] = KING_HEARTS;
    game.foundation[DIAMONDS] = KING_DIAMONDS;
    game.foundation[CLUBS] = KING_CLUBS;

    game.reserve[0] = ACE_SPADES;

    assert(!freecell_game_over(&game));

    print_test_result("test_freecell_game_not_over_due_to_reserve", true);
}

void test_freecell_game_not_over_due_to_cascade(void) {
    Freecell game = { 0 };

    game.foundation[SPADES] = KING_SPADES;
    game.foundation[HEARTS] = KING_HEARTS;
    game.foundation[DIAMONDS] = KING_DIAMONDS;
    game.foundation[CLUBS] = KING_CLUBS;

    game.cascade[0].cards[0] = ACE_HEARTS;
    game.cascade[0].size = 1;

    assert(!freecell_game_over(&game));

    print_test_result("test_freecell_game_not_over_due_to_cascade", true);
}

void test_freecell_game_not_over_due_to_foundation_missing(void) {
    Freecell game = { 0 };

    game.foundation[SPADES] = KING_SPADES;
    game.foundation[HEARTS] = KING_HEARTS;
    game.foundation[DIAMONDS] = KING_DIAMONDS;

    assert(!freecell_game_over(&game));

    print_test_result("test_freecell_game_not_over_due_to_foundation_missing", true);
}

void test_freecell_validate_move_invalid_size_zero(void) {
    Freecell game = { 0 };
    Move move = {
        .from = CASCADE_1,
        .to = CASCADE_2,
        .size = 0,
    };

    MoveResult result = freecell_validate_move(&game, move);
    assert(result == MOVE_ERROR);

    print_test_result("test_freecell_validate_move_invalid_size_zero", true);
}

void test_freecell_validate_move_invalid_same_source_dest(void) {
    Freecell game = { 0 };
    Move move = {
        .from = CASCADE_1,
        .to = CASCADE_1,
        .size = 1,
    };

    MoveResult result = freecell_validate_move(&game, move);
    assert(result == MOVE_ERROR);

    print_test_result("test_freecell_validate_move_invalid_same_source_dest", true);
}

void test_cascade_is_stacked_properly(void) {
    Cascade c = { .size = 4 };
    c.cards[0] = EIGHT_SPADES; // Black
    c.cards[1] = SEVEN_HEARTS; // Red
    c.cards[2] = SIX_CLUBS; // Black
    c.cards[3] = FIVE_DIAMONDS; // Red

    assert(cascade_is_stacked_properly(&c, 0));
    print_test_result("cascade_is_stacked_properly - valid", true);

    c.cards[2] = SIX_SPADES; // Same color as previous (black), invalid
    assert(!cascade_is_stacked_properly(&c, 0));
    print_test_result("cascade_is_stacked_properly - invalid color", true);

    c.cards[2] = FOUR_CLUBS; // Wrong rank (should be 6)
    assert(!cascade_is_stacked_properly(&c, 0));
    print_test_result("cascade_is_stacked_properly - invalid rank", true);
}

void test_freecell_is_trivially_solved_true(void) {
    Freecell f = { 0 };

    for (int i = 0; i < 8; i++) {
        Cascade* c = &f.cascade[i];
        c->size = 3;
        c->cards[0] = SEVEN_SPADES;
        c->cards[1] = SIX_HEARTS;
        c->cards[2] = FIVE_CLUBS;
    }

    assert(freecell_is_trivially_solved(&f));
    print_test_result("freecell_is_trivially_solved - valid stacks", true);
}

void test_freecell_is_trivially_solved_false(void) {
    Freecell f = { 0 };

    for (int i = 0; i < 8; i++) {
        Cascade* c = &f.cascade[i];
        c->size = 3;
        c->cards[0] = SEVEN_SPADES;
        c->cards[1] = SIX_CLUBS; // same color as previous (invalid)
        c->cards[2] = FIVE_DIAMONDS;
    }

    assert(!freecell_is_trivially_solved(&f));
    print_test_result("freecell_is_trivially_solved - invalid stacks", true);
}

int main(void) {
    test_freecell_push_and_pop_cascade();
    test_suits_differ_by_color();
    test_cascade_is_stacked_properly();
    test_freecell_init_distribution();
    test_freecell_move_to_foundation();
    test_freecell_move_to_reserve();
    test_freecell_move_to_cascade_single();

    test_freecell_move_to_foundation_wrong_rank();
    test_freecell_move_to_reserve_full_slot();
    test_freecell_push_pop_multiple_cascade();
    test_suits_differ_by_color_same_suit();
    test_freecell_move_to_cascade_invalid_rank_gap();
    test_freecell_move_reserve_to_cascade();
    test_freecell_move_to_foundation_invalid_start();

    test_freecell_valid_multi_card_cascade_move();
    test_freecell_invalid_multi_card_wrong_color();
    test_freecell_invalid_move_exceed_max_moves();
    test_freecell_reserve_to_cascade_wrong_suit();
    test_freecell_game_over_check();
    test_freecell_is_trivially_solved_true();
    test_freecell_is_trivially_solved_false();

    test_freecell_game_not_over_due_to_reserve();
    test_freecell_game_not_over_due_to_cascade();
    test_freecell_game_not_over_due_to_foundation_missing();
    test_freecell_validate_move_invalid_size_zero();
    test_freecell_validate_move_invalid_same_source_dest();

    printf("All tests completed.\n");
    return 0;
}
