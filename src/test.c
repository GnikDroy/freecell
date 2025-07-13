#include "game.c"
#include <assert.h>
#include <stdio.h>

void print_test_result(const char *test_name, bool passed) {
  printf("%s: %s\n", test_name, passed ? "PASS" : "FAIL");
}

void test_shuffle_deck_changes_order(void) {
  Deck deck1, deck2;
  for (int i = 0; i < 52; i++) {
    deck1[i] = i + 1;
    deck2[i] = i + 1;
  }
  shuffle_deck(deck1);

  bool order_changed = false;
  for (int i = 0; i < 52; i++) {
    if (deck1[i] != deck2[i]) {
      order_changed = true;
      break;
    }
  }
  print_test_result("test_shuffle_deck_changes_order", order_changed);
  assert(order_changed);
}

void test_freecell_push_and_pop_cascade(void) {
  Cascade c = {.size = 0};
  Card card = 5;

  uint8_t size_after_push = freecell_push_cascade(&c, card);
  assert(size_after_push == 1);
  assert(c.cards[0] == card);

  Card popped = freecell_pop_cascade(&c);
  assert(popped == card);
  assert(c.size == 0);

  popped = freecell_pop_cascade(&c);
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
  Freecell game = {0};
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
  Freecell game = {0};
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
  Freecell game = {0};
  Cascade *c = &game.cascade[0];

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

void test_freecell_move(void) {
  Game game = game_init();

  Move move = {.from = CASCADE_1, .to = RESERVE_1, .size = 1};
  MoveResult res = game_move(&game, move);
  assert(res == MOVE_SUCCESS);
  assert(game.move_count == 1);

  game_undo(&game);
  assert(game.move_count == 1);

  print_test_result("test_freecell_move_and_undo", true);
}

void test_freecell_move_to_foundation_wrong_rank(void) {
  Freecell game = {0};
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
  Freecell game = {0};
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
  Cascade c = {.size = 0};
  Card cards[] = {ACE_HEARTS, TWO_SPADES, THREE_DIAMONDS};

  for (int i = 0; i < 3; i++) {
    uint8_t size_after_push = freecell_push_cascade(&c, cards[i]);
    assert(size_after_push == i + 1);
  }

  for (int i = 2; i >= 0; i--) {
    Card popped = freecell_pop_cascade(&c);
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
  Freecell game = {0};

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
  Freecell game = {0};

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
  Freecell game = {0};
  Card two_spades = TWO_SPADES;

  MoveResult res = freecell_validate_to_foundation(&game, two_spades);
  assert(res == MOVE_ERROR);

  print_test_result("test_freecell_move_to_foundation_invalid_start", true);
}

int main(void) {
  test_shuffle_deck_changes_order();
  test_freecell_push_and_pop_cascade();
  test_freecell_init_distribution();
  test_suits_differ_by_color();
  test_freecell_move_to_foundation();
  test_freecell_move_to_reserve();
  test_freecell_move_to_cascade_single();
  test_freecell_move();

  test_freecell_move_to_foundation_wrong_rank();
  test_freecell_move_to_reserve_full_slot();
  test_freecell_push_pop_multiple_cascade();
  test_suits_differ_by_color_same_suit();
  test_freecell_move_to_cascade_invalid_rank_gap();
  test_freecell_move_reserve_to_cascade();
  test_freecell_move_to_foundation_invalid_start();

  printf("All tests completed.\n");
  return 0;
}
