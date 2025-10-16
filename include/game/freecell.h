#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "card.h"

typedef struct Cascade {
    uint8_t size;

    // one cascade can only hold upto 19 cards.
    // 7 initial cards & 12 additional cards if the bottom card is a king.
    Card cards[19];
} Cascade;

typedef struct Freecell {
    Card reserve[4];
    Card foundation[4];
    Cascade cascade[8];
} Freecell;

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

bool suits_differ_by_color(Suit suit1, Suit suit2);

uint8_t cascade_push(Cascade* cascade, Card card);

Card cascade_pop(Cascade* cascade);

bool cascade_is_stacked_properly(Cascade* cascade, size_t start_index);

bool cascade_is_descending(Cascade* cascade, size_t start_index);

Freecell freecell_init(uint32_t seed);

bool freecell_game_over(Freecell* freecell);

bool freecell_is_trivially_solved(Freecell* freecell);

/** Counts the number of cards in a cascade starting from a given index.
 *
 * @param freecell The Freecell game state.
 * @param location The selection location (cascade, reserve, or foundation).
 * @param card_index The index of the card in the cascade.
 *
 * @return The number of cards in the cascade starting from the given index.
 */
uint8_t freecell_count_cards_from_index(
    const Freecell* freecell,
    SelectionLocation location,
    uint32_t card_index
);

/** Gets the index of the first card in a cascade based on the number of cards being moved.
 * If location is not a cascade, it returns 0.
 *
 * @param freecell The Freecell game state.
 * @param location The selection location (cascade, reserve, or foundation).
 * @param size The number of cards being moved.
 *
 * @return The index of the first card in the cascade for the given size.
 */
uint8_t freecell_get_index_from_size(
    const Freecell* freecell,
    SelectionLocation location,
    uint8_t size
);

inline Card freecell_get_card(
    const Freecell* freecell,
    SelectionLocation location,
    uint8_t card_index
) {
    if (selection_location_is_foundation(location)) {
        return freecell->foundation[location - FOUNDATION_SPADES];
    } else if (selection_location_is_reserve(location)) {
        return freecell->reserve[location - RESERVE_1];
    } else {
        return freecell->cascade[location - CASCADE_1].cards[card_index];
    }
}

uint8_t freecell_count_max_moves(Freecell* freecell);

uint64_t freecell_move_count(Freecell* freecell, Move move);

MoveResult freecell_validate_to_foundation(Freecell* freecell, Card card, SelectionLocation dest);

MoveResult freecell_validate_to_reserve(Freecell* freecell, Card card, SelectionLocation dest);

MoveResult freecell_validate_to_cascade_single(
    Freecell* freecell,
    Card card,
    SelectionLocation dest
);

MoveResult freecell_validate_to_cascade(Freecell* freecell, Move move);

MoveResult freecell_validate_move(Freecell* freecell, Move move);

void freecell_move_to_foundation(Freecell* freecell, Card card, SelectionLocation dest);

void freecell_move_to_reserve(Freecell* freecell, Card card, SelectionLocation dest);

void freecell_move_to_cascade_single(Freecell* freecell, Card card, SelectionLocation dest);

void freecell_move_to_cascade(Freecell* freecell, Move move);

void freecell_move(Freecell* freecell, Move move);
