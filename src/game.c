#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

Game game_init(void) {
  Game game = {
      .freecell = freecell_init(),
      .move_count = 0,
      .history = vec_init(sizeof(Move)),
  };
  return game;
}

void game_free(Game *game) { vec_free(&game->history); }

void game_new(Game *game) {
  game->freecell = freecell_init();
  game->move_count = 0;
  game->history.size = 0;
}

bool game_can_move_from(Game *game, SelectionLocation from,
                        uint32_t card_index) {
  Freecell *freecell = &game->freecell;

  if (selection_location_is_foundation(from)) {
    return freecell->foundation[from - FOUNDATION_SPADES] != NONE;
  } else if (selection_location_is_reserve(from)) {
    return freecell->reserve[from - RESERVE_1] != NONE;
  } else if (selection_location_is_cascade(from)) {
    uint32_t cascade_idx = from - CASCADE_1;
    Cascade *cascade = &freecell->cascade[cascade_idx];

    if (cascade->size == 0) {
      return false;
    }

    if (card_index == cascade->size - 1) {
      return true;
    }

    // super moves
    uint8_t cards_moved = cascade->size - card_index;
    if (cards_moved > freecell_count_max_moves(freecell)) {
      return false;
    }

    // every card below must be alternating color and one rank lower
    Rank prev_rank = get_rank(cascade->cards[card_index]);
    Suit prev_suit = get_suit(cascade->cards[card_index]);
    for (uint8_t i = card_index + 1; i < cascade->size; i++) {
      Card card = cascade->cards[i];
      Rank rank = get_rank(card);
      Suit suit = get_suit(card);
      if (rank != prev_rank - 1 || !suits_differ_by_color(prev_suit, suit)) {
        return false;
      }
      prev_rank = rank;
      prev_suit = suit;
    }

    return true;
  }

  return false;
}

MoveResult game_move(Game *game, Move move) {
  MoveResult result = freecell_validate_move(&game->freecell, move);
  if (result == MOVE_SUCCESS) {
    freecell_move(&game->freecell, move);
    game->move_count++;
    vec_push_back(&game->history, &move);
  }
  return result;
}

static Move move_get_reverse(Move move) {
  Move reverse_move = {
      .from = move.to,
      .to = move.from,
      .size = move.size,
  };
  return reverse_move;
}

void game_undo(Game *game) {
  if (game->history.size == 0) {
    return;
  }

  Move last_move;
  void *move = vec_get(&game->history, game->history.size - 1);
  memcpy(&last_move, move, sizeof(Move));

  Move reverse_move = move_get_reverse(last_move);
  freecell_move(&game->freecell, reverse_move);

  vec_pop_back(&game->history);
  game->move_count++; // Freecell rules allow undoing moves, so we increment the
                      // move count
}
