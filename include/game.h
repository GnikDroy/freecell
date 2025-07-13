#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "vector.h"

typedef uint8_t Suit;
enum {
  SPADES,
  HEARTS,
  DIAMONDS,
  CLUBS,
};

typedef uint8_t Rank;
enum {
  ACE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
  JACK,
  QUEEN,
  KING,
};

typedef uint8_t Card;
enum {
  NONE = 0,

  // SPADES
  ACE_SPADES,
  TWO_SPADES,
  THREE_SPADES,
  FOUR_SPADES,
  FIVE_SPADES,
  SIX_SPADES,
  SEVEN_SPADES,
  EIGHT_SPADES,
  NINE_SPADES,
  TEN_SPADES,
  JACK_SPADES,
  QUEEN_SPADES,
  KING_SPADES,

  // HEARTS
  ACE_HEARTS,
  TWO_HEARTS,
  THREE_HEARTS,
  FOUR_HEARTS,
  FIVE_HEARTS,
  SIX_HEARTS,
  SEVEN_HEARTS,
  EIGHT_HEARTS,
  NINE_HEARTS,
  TEN_HEARTS,
  JACK_HEARTS,
  QUEEN_HEARTS,
  KING_HEARTS,

  // DIAMONDS
  ACE_DIAMONDS,
  TWO_DIAMONDS,
  THREE_DIAMONDS,
  FOUR_DIAMONDS,
  FIVE_DIAMONDS,
  SIX_DIAMONDS,
  SEVEN_DIAMONDS,
  EIGHT_DIAMONDS,
  NINE_DIAMONDS,
  TEN_DIAMONDS,
  JACK_DIAMONDS,
  QUEEN_DIAMONDS,
  KING_DIAMONDS,

  // CLUBS
  ACE_CLUBS,
  TWO_CLUBS,
  THREE_CLUBS,
  FOUR_CLUBS,
  FIVE_CLUBS,
  SIX_CLUBS,
  SEVEN_CLUBS,
  EIGHT_CLUBS,
  NINE_CLUBS,
  TEN_CLUBS,
  JACK_CLUBS,
  QUEEN_CLUBS,
  KING_CLUBS,
};

inline Suit get_suit(Card card) { return (Suit)((card - 1) / 13); }

inline Rank get_rank(Card card) { return (Rank)((card - 1) % 13); }

typedef struct Cascade {
  uint8_t size;

  // one cascade can only hold upto 19 cards.
  // 7 initial cards & 12 additional cards if the bottom card is a king.
  Card cards[19];
} Cascade;

typedef uint8_t SelectionLocation;
enum {
  CASCADE_1,
  CASCADE_2,
  CASCADE_3,
  CASCADE_4,
  CASCADE_5,
  CASCADE_6,
  CASCADE_7,
  CASCADE_8,
  RESERVE_1,
  RESERVE_2,
  RESERVE_3,
  RESERVE_4,
  FOUNDATION_SPADES,
  FOUNDATION_HEARTS,
  FOUNDATION_DIAMONDS,
  FOUNDATION_CLUBS,
};

inline bool selection_location_is_cascade(SelectionLocation location) {
  return location >= CASCADE_1 && location <= CASCADE_8;
}

inline bool selection_location_is_reserve(SelectionLocation location) {
  return location >= RESERVE_1 && location <= RESERVE_4;
}

inline bool selection_location_is_foundation(SelectionLocation location) {
  return location >= FOUNDATION_SPADES && location <= FOUNDATION_CLUBS;
}

typedef struct Move {
  SelectionLocation from;
  SelectionLocation to;
  uint8_t size;
} Move;

typedef uint8_t MoveResult;
enum {
  MOVE_SUCCESS,
  MOVE_ERROR_WRONG_RANK,
  MOVE_ERROR_WRONG_SUIT,
  MOVE_ERROR_INSUFFICIENT_FREECELLS,
  MOVE_ERROR,
};

typedef struct Freecell {
  Card reserve[4];
  Card foundation[4];
  Cascade cascade[8];
} Freecell;

typedef struct Game {
  Freecell freecell;
  size_t move_count;
  Vector history;
} Game;

Game game_init(void);

void game_free(Game *game);

void game_new(Game *game);

bool game_can_move_from(Game *game, SelectionLocation from, uint32_t card_index);

MoveResult game_move(Game *game, Move move);

void game_undo(Game *game);
