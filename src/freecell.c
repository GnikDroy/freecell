#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

static void shuffle_deck(Card deck[]) {
  for (int i = 51; i > 0; i--) {
    int j = rand() % (i + 1);
    Card temp = deck[i];
    deck[i] = deck[j];
    deck[j] = temp;
  }
}

uint8_t cascade_push(Cascade *cascade, Card card) {
  cascade->cards[cascade->size++] = card;
  return cascade->size;
}

Card cascade_pop(Cascade *cascade) {
  if (cascade->size == 0) {
    return NONE;
  }
  Card popped = cascade->cards[cascade->size - 1];
  cascade->size--;
  return popped;
}

Freecell freecell_init(void) {
  Freecell game = {0};

  // Get a random deck
  Card deck[52];
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

bool freecell_game_over(Freecell *freecell) {
  for (int i = 0; i < 4; i++) {
    if (freecell->reserve[i] != NONE) {
      return false;
    }
  }

  for (int i = 0; i < 8; i++) {
    if (freecell->cascade[i].size > 0) {
      return false;
    }
  }

  return freecell->foundation[SPADES] =
             ACE_SPADES && freecell->foundation[HEARTS] == ACE_HEARTS &&
             freecell->foundation[DIAMONDS] == ACE_DIAMONDS &&
             freecell->foundation[CLUBS] == ACE_CLUBS;
}

bool suits_differ_by_color(Suit suit1, Suit suit2) {
  return ((suit1 == SPADES || suit1 == CLUBS) &&
          (suit2 == HEARTS || suit2 == DIAMONDS)) ||
         ((suit1 == HEARTS || suit1 == DIAMONDS) &&
          (suit2 == SPADES || suit2 == CLUBS));
}

uint8_t freecell_count_max_moves(Freecell *freecell) {
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

MoveResult freecell_validate_to_foundation(Freecell *freecell, Card card) {
  Suit card_suit = get_suit(card);
  Rank card_rank = get_rank(card);

  Card *foundation = &freecell->foundation[card_suit];
  if (*foundation == NONE && card_rank == ACE) {
    return MOVE_SUCCESS;
  } else {
    Rank foundation_rank = get_rank(*foundation);
    if (foundation_rank == card_rank - 1) {
      return MOVE_SUCCESS;
    }
  }

  return MOVE_ERROR;
}

MoveResult freecell_validate_to_reserve(Freecell *freecell, Card card,
                                        SelectionLocation dest) {
  if (card == NONE) {
    return MOVE_ERROR;
  }

  if (freecell->reserve[dest - RESERVE_1] != NONE) {
    return MOVE_ERROR;
  }

  return MOVE_SUCCESS;
}

MoveResult freecell_validate_to_cascade_single(Freecell *freecell, Card card,
                                               SelectionLocation dest) {
  Cascade *cascade = &freecell->cascade[dest - CASCADE_1];

  if (cascade->size == 0) {
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
    return MOVE_SUCCESS;
  }
}

MoveResult freecell_validate_to_cascade(Freecell *freecell, Move move) {
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
  }

  return MOVE_SUCCESS;
}

MoveResult freecell_validate_move(Freecell *freecell, Move move) {
  if (move.from == move.to || move.size <= 0) {
    return MOVE_ERROR;
  }

  bool from_cascade = selection_location_is_cascade(move.from);
  bool to_cascade = selection_location_is_cascade(move.to);

  bool from_reserve = selection_location_is_reserve(move.from);
  bool to_reserve = selection_location_is_reserve(move.to);

  bool from_foundation = selection_location_is_foundation(move.from);
  bool to_foundation = selection_location_is_foundation(move.to);

  if (from_foundation && to_foundation) {
    return MOVE_ERROR;
  } else if (from_foundation && to_reserve) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    return freecell_validate_to_reserve(freecell, card, move.to);
  } else if (from_foundation && to_cascade) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    return freecell_validate_to_cascade_single(freecell, card, move.to);
  } else if (from_reserve && to_foundation) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    return freecell_validate_to_foundation(freecell, card);
  } else if (from_reserve && to_reserve) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    return freecell_validate_to_reserve(freecell, card, move.to);
  } else if (from_reserve && to_cascade) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    return freecell_validate_to_cascade_single(freecell, card, move.to);
  } else if (from_cascade && to_foundation && move.size == 1) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    return freecell_validate_to_foundation(freecell, card);
  } else if (from_cascade && to_reserve) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    return freecell_validate_to_reserve(freecell, card, move.to);
  } else if (from_cascade && to_cascade) {
    return freecell_validate_to_cascade(freecell, move);
  }

  return MOVE_SUCCESS;
}

void freecell_move_to_foundation(Freecell *freecell, Card card) {
  Card *foundation = &freecell->foundation[get_suit(card)];
  *foundation = card;
}

void freecell_move_to_reserve(Freecell *freecell, Card card,
                              SelectionLocation dest) {
  freecell->reserve[dest - RESERVE_1] = card;
}

void freecell_move_to_cascade_single(Freecell *freecell, Card card,
                                     SelectionLocation dest) {
  Cascade *cascade = &freecell->cascade[dest - CASCADE_1];
  cascade_push(cascade, card);
}

void freecell_move_to_cascade(Freecell *freecell, Move move) {
  Cascade *from_cascade = &freecell->cascade[move.from];
  Cascade *to_cascade = &freecell->cascade[move.to];

  memcpy(to_cascade->cards + to_cascade->size,
         from_cascade->cards + from_cascade->size - move.size, move.size);
  from_cascade->size -= move.size;
  to_cascade->size += move.size;
}

void freecell_move(Freecell *freecell, Move move) {
  bool from_cascade = selection_location_is_cascade(move.from);
  bool to_cascade = selection_location_is_cascade(move.to);

  bool from_reserve = selection_location_is_reserve(move.from);
  bool to_reserve = selection_location_is_reserve(move.to);

  bool from_foundation = selection_location_is_foundation(move.from);
  bool to_foundation = selection_location_is_foundation(move.to);

  if (from_foundation && to_foundation) {
  } else if (from_foundation && to_reserve) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    freecell_move_to_reserve(freecell, card, move.to);
    freecell->foundation[move.from - FOUNDATION_SPADES] =
        get_rank(card) == ACE ? NONE : (card - 1);
  } else if (from_foundation && to_cascade) {
    Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
    freecell_move_to_cascade_single(freecell, card, move.to);
    freecell->foundation[move.from - FOUNDATION_SPADES] =
        get_rank(card) == ACE ? NONE : (card - 1);
  } else if (from_reserve && to_foundation) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    freecell_move_to_foundation(freecell, card);
    freecell->reserve[move.from - RESERVE_1] = NONE;
  } else if (from_reserve && to_reserve) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    freecell_move_to_reserve(freecell, card, move.to);
    freecell->reserve[move.from - RESERVE_1] = NONE;
  } else if (from_reserve && to_cascade) {
    Card card = freecell->reserve[move.from - RESERVE_1];
    freecell_move_to_cascade_single(freecell, card, move.to);
    freecell->reserve[move.from - RESERVE_1] = NONE;
  } else if (from_cascade && to_foundation && move.size == 1) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    freecell_move_to_foundation(freecell, card);
    cascade_pop(cascade);
  } else if (from_cascade && to_reserve) {
    Cascade *cascade = &freecell->cascade[move.from - CASCADE_1];
    Card card = cascade->cards[cascade->size - move.size];
    freecell_move_to_reserve(freecell, card, move.to);
    cascade_pop(cascade);
  } else if (from_cascade && to_cascade) {
    freecell_move_to_cascade(freecell, move);
  }
}
