#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

typedef Card Deck[52];

static void shuffle_deck(Deck deck) {
  for (int i = 51; i > 0; i--) {
    int j = rand() % (i + 1);
    Card temp = deck[i];
    deck[i] = deck[j];
    deck[j] = temp;
  }
}

static uint8_t freecell_push_cascade(Cascade *cascade, Card card) {
  cascade->cards[cascade->size++] = card;
  return cascade->size;
}

static Card freecell_pop_cascade(Cascade *cascade) {
  if (cascade->size == 0) {
    return NONE;
  }
  Card popped = cascade->cards[cascade->size - 1];
  cascade->size--;
  return popped;
}

static Freecell freecell_init(void) {
  Freecell game = {0};

  // Get a random deck
  Deck deck;
  for (int i = 0; i < 52; i++)
    deck[i] = i + 1;
  shuffle_deck(deck);

  // First 4 cascades will have 7 cards each
  uint8_t deck_idx = 0;
  for (int i = 0; i < 4; i++) {
    memcpy(game.cascade[i].cards, deck + deck_idx, 7);
    game.cascade[i].size = 7;
    deck_idx += 7;
  }

  // Remaining 4 cascades will have 6 cards each
  for (int i = 4; i < 8; i++) {
    memcpy(game.cascade[i].cards, deck + deck_idx, 6);
    game.cascade[i].size = 6;
    deck_idx += 6;
  }

  return game;
}

static bool suits_differ_by_color(Suit suit1, Suit suit2) {
  return ((suit1 == SPADES || suit1 == CLUBS) &&
          (suit2 == HEARTS || suit2 == DIAMONDS)) ||
         ((suit1 == HEARTS || suit1 == DIAMONDS) &&
          (suit2 == SPADES || suit2 == CLUBS));
}

static MoveResult freecell_move_to_foundation(Freecell *freecell, Card card) {
  Suit card_suit = get_suit(card);
  Rank card_rank = get_rank(card);

  Card *foundation = &freecell->foundation[card_suit];
  if (*foundation == NONE && card_rank == ACE) {
    *foundation = card;
    return MOVE_SUCCESS;
  } else {
    Rank foundation_rank = get_rank(*foundation);
    if (foundation_rank == card_rank - 1) {
      *foundation = card;
      return MOVE_SUCCESS;
    }
  }

  return MOVE_ERROR;
}

static MoveResult freecell_move_to_reserve(Freecell *freecell, Card card,
                                           SelectionLocation dest) {
  if (card == NONE) {
    return MOVE_ERROR;
  }

  if (freecell->reserve[dest - RESERVE_1] != NONE) {
    return MOVE_ERROR;
  }

  freecell->reserve[dest - RESERVE_1] = card;
  return MOVE_SUCCESS;
}

static MoveResult freecell_move_to_cascade_single(Freecell *freecell, Card card,
                                                  SelectionLocation dest) {
  Cascade *cascade = &freecell->cascade[dest - CASCADE_1];

  if (cascade->size == 0) {
    freecell_push_cascade(cascade, card);
    return MOVE_SUCCESS;
  }

  Card to_card = cascade->cards[cascade->size - 1];

  Suit from_suit = get_suit(card);
  Rank from_rank = get_rank(card);

  Suit to_suit = get_suit(to_card);
  Rank to_rank = get_rank(to_card);

  if (!suits_differ_by_color(from_suit, to_suit)) {
    return MOVE_ERROR_WRONG_SUIT;
  } else if (from_rank != to_rank - 1) {
    return MOVE_ERROR_WRONG_RANK;
  } else {
    freecell_push_cascade(cascade, card);
    return MOVE_SUCCESS;
  }
}

static uint8_t freecell_count_max_moves(Freecell *freecell) {
  uint8_t empty_cascades = 0;
  for (int i = 0; i < 8; i++) {
    if (freecell->cascade[i].size == 0) {
      empty_cascades++;
    }
  }

  uint8_t freecells = 0;
  for (int i = 0; i < 4; i++) {
    if (freecell->reserve[i] == NONE) {
      freecells++;
    }
  }

  return (1 << empty_cascades) * (freecells + 1);
}

static MoveResult freecell_move_to_cascade(Freecell *freecell, Move move) {
  int max_moves = freecell_count_max_moves(freecell);

  if (freecell->cascade[move.to].size == 0) {
    max_moves /= 2;
  }

  if (move.size > max_moves) {
    return MOVE_ERROR_INSUFFICIENT_FREECELLS;
  }

  Cascade *from_cascade = &freecell->cascade[move.from];
  Cascade *to_cascade = &freecell->cascade[move.to];

  if (move.size > from_cascade->size) {
    return MOVE_ERROR;
  }

  // move to empty cascade
  if (to_cascade->size == 0) {
    memcpy(to_cascade->cards,
           from_cascade->cards + from_cascade->size - move.size, move.size);
    from_cascade->size -= move.size;
    to_cascade->size += move.size;
    return MOVE_SUCCESS;
  }

  // Find the top card of source
  Card from_top_card = from_cascade->cards[from_cascade->size - move.size];
  Card to_card = to_cascade->cards[to_cascade->size - 1];

  Suit from_suit = get_suit(from_top_card);
  Rank from_rank = get_rank(from_top_card);

  Suit to_suit = get_suit(to_card);
  Rank to_rank = get_rank(to_card);

  if (!suits_differ_by_color(from_suit, to_suit)) {
    return MOVE_ERROR_WRONG_SUIT;
  } else if (from_rank != to_rank - 1) {
    return MOVE_ERROR_WRONG_RANK;
  } else {
    memcpy(to_cascade->cards + to_cascade->size,
           from_cascade->cards + from_cascade->size - move.size, move.size);
    from_cascade->size -= move.size;
    to_cascade->size += move.size;
    return MOVE_SUCCESS;
  }
}

static MoveResult freecell_move(Freecell *freecell, Move move) {
  if (move.from == move.to || move.size <= 0) {
    return MOVE_ERROR;
  }

  bool from_cascade = selection_location_is_cascade(move.from);
  bool to_cascade = selection_location_is_cascade(move.to);

  bool from_reserve = selection_location_is_reserve(move.from);
  bool to_reserve = selection_location_is_reserve(move.to);

  bool from_foundation = selection_location_is_foundation(move.from);
  bool to_foundation = selection_location_is_foundation(move.to);

  MoveResult result = MOVE_SUCCESS;

  if (from_foundation && to_foundation) {
    result = MOVE_ERROR;
  } else if (from_foundation && to_reserve) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    result = freecell_move_to_reserve(freecell, card, move.to);
    if (result == MOVE_SUCCESS) {
      freecell->foundation[move.from - FOUNDATION_SPADES] =
          get_rank(card) == ACE ? NONE : (card - 1);
    }
  } else if (from_foundation && to_cascade) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    result = freecell_move_to_cascade_single(freecell, card, move.to);
    if (result == MOVE_SUCCESS) {
      freecell->foundation[move.from - FOUNDATION_SPADES] =
          get_rank(card) == ACE ? NONE : (card - 1);
    }
  } else if (from_reserve && to_foundation) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    result = freecell_move_to_foundation(freecell, card);
    if (result == MOVE_SUCCESS) {
      freecell->reserve[move.from - RESERVE_1] = NONE;
    }
  } else if (from_reserve && to_reserve) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    result = freecell_move_to_reserve(freecell, card, move.to);
    if (result == MOVE_SUCCESS) {
      freecell->reserve[move.from - RESERVE_1] = NONE;
    }
  } else if (from_reserve && to_cascade) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    result = freecell_move_to_cascade_single(freecell, card, move.to);
    if (result == MOVE_SUCCESS) {
      freecell->reserve[move.from - RESERVE_1] = NONE;
    }
  } else if (from_cascade && to_foundation && move.size == 1) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    result = freecell_move_to_foundation(freecell, card);
    if (result == MOVE_SUCCESS) {
      freecell_pop_cascade(cascade);
    }
  } else if (from_cascade && to_reserve) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    result = freecell_move_to_reserve(freecell, card, move.to);
    if (result == MOVE_SUCCESS) {
      freecell_pop_cascade(cascade);
    }
  } else if (from_cascade && to_cascade) {
    result = freecell_move_to_cascade(freecell, move);
  }

  return result;
}

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
  MoveResult result = freecell_move(&game->freecell, move);
  if (result == MOVE_SUCCESS) {
    printf("Move success %d to %d of size %d\n", move.from, move.to, move.size);
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
  MoveResult result = freecell_move(&game->freecell, reverse_move);

  if (result != MOVE_SUCCESS) {
    printf("Undo failed with error code: %d\n", result);
    return;
  }

  vec_pop_back(&game->history);
  game->move_count++; // Freecell rules allow undoing moves, so we increment the
                      // move count
}
