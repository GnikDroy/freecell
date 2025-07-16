#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "core/game.h"

static int seed = 1;

inline int microsoft_freecell_rand(void) {
    uint32_t mask = (1U << 31) - 1;
    seed = (seed * 214013 + 2531011) & mask;
    return seed >> 16;
}

inline void microsoft_freecell_srand(uint32_t x) { seed = x; }

inline void microsoft_freecell_shuffle(Card cards[]) {

    // clang-format off
    Card initial_deck[] = {
          ACE_CLUBS,   ACE_DIAMONDS,   ACE_HEARTS,   ACE_SPADES,
          TWO_CLUBS,   TWO_DIAMONDS,   TWO_HEARTS,   TWO_SPADES,
        THREE_CLUBS, THREE_DIAMONDS, THREE_HEARTS, THREE_SPADES,
         FOUR_CLUBS,  FOUR_DIAMONDS,  FOUR_HEARTS,  FOUR_SPADES,
         FIVE_CLUBS,  FIVE_DIAMONDS,  FIVE_HEARTS,  FIVE_SPADES,
          SIX_CLUBS,   SIX_DIAMONDS,   SIX_HEARTS,   SIX_SPADES,
        SEVEN_CLUBS, SEVEN_DIAMONDS, SEVEN_HEARTS, SEVEN_SPADES,
        EIGHT_CLUBS, EIGHT_DIAMONDS, EIGHT_HEARTS, EIGHT_SPADES,
         NINE_CLUBS,  NINE_DIAMONDS,  NINE_HEARTS,  NINE_SPADES,
          TEN_CLUBS,   TEN_DIAMONDS,   TEN_HEARTS,   TEN_SPADES,
         JACK_CLUBS,  JACK_DIAMONDS,  JACK_HEARTS,  JACK_SPADES,
        QUEEN_CLUBS, QUEEN_DIAMONDS, QUEEN_HEARTS, QUEEN_SPADES,
         KING_CLUBS,  KING_DIAMONDS,  KING_HEARTS,  KING_SPADES,
    };
    // clang-format on

    for (uint32_t i = 0; i < 52; i++)
        cards[i] = initial_deck[51 - i];

    for (uint32_t i = 0; i < 52; i++) {
        uint32_t j = 51 - microsoft_freecell_rand() % (52 - i);

        uint32_t tmp = cards[j];
        cards[j] = cards[i];
        cards[i] = tmp;
    }
}

bool suits_differ_by_color(Suit suit1, Suit suit2) {
    return ((suit1 == SPADES || suit1 == CLUBS) && (suit2 == HEARTS || suit2 == DIAMONDS))
        || ((suit1 == HEARTS || suit1 == DIAMONDS) && (suit2 == SPADES || suit2 == CLUBS));
}

uint8_t cascade_push(Cascade* cascade, Card card) {
    cascade->cards[cascade->size++] = card;
    return cascade->size;
}

Card cascade_pop(Cascade* cascade) {
    if (cascade->size == 0) {
        return NONE;
    }
    Card popped = cascade->cards[cascade->size - 1];
    cascade->size--;
    return popped;
}

bool cascade_is_stacked_properly(Cascade* cascade, size_t start_index) {
    if (cascade->size == 0) {
        return true;
    }

    if (start_index >= cascade->size) {
        return false;
    }

    Rank prev_rank = get_rank(cascade->cards[start_index]);
    Suit prev_suit = get_suit(cascade->cards[start_index]);

    for (size_t i = start_index + 1; i < cascade->size; i++) {
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

bool cascade_is_descending(Cascade* cascade, size_t start_index) {
    if (cascade->size == 0) {
        return true;
    }

    if (start_index >= cascade->size) {
        return false;
    }

    Rank prev_rank = get_rank(cascade->cards[start_index]);
    for (size_t i = start_index + 1; i < cascade->size; i++) {
        Card card = cascade->cards[i];
        Rank rank = get_rank(card);
        if (rank > prev_rank) {
            return false;
        }
        prev_rank = rank;
    }

    return true;
}

Freecell freecell_init(void) {
    Freecell game = { 0 };

    // Get a random deck
    Card deck[52];

    struct timespec now;
    timespec_get(&now, TIME_UTC);
    uint64_t seed = (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;

    microsoft_freecell_srand(seed % 10000000 + 1);
    microsoft_freecell_shuffle(deck);

    // Deal row by row first 6 rows
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            cascade_push(&game.cascade[j], deck[i * 8 + j]);
        }
    }

    // Deal the last four cards
    for (int i = 0; i < 4; i++) {
        cascade_push(&game.cascade[i], deck[48 + i]);
    }

    return game;
}

bool freecell_game_over(Freecell* freecell) {
    for (int i = 0; i < 4; i++) {
        if (freecell->reserve[i] != NONE) {
            return false;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (freecell->cascade[i].size != 0) {
            return false;
        }
    }

    return freecell->foundation[SPADES] == KING_SPADES
        && freecell->foundation[HEARTS] == KING_HEARTS
        && freecell->foundation[DIAMONDS] == KING_DIAMONDS
        && freecell->foundation[CLUBS] == KING_CLUBS;
}

bool freecell_is_trivially_solved(Freecell* freecell) {
    if (freecell_game_over(freecell)) {
        return true;
    }

    for (uint8_t i = 0; i < 8; i++) {
        if (!cascade_is_descending(&freecell->cascade[i], 0)) {
            return false;
        }
    }

    return true;
}

uint8_t freecell_count_cards_from_index(
    const Freecell* freecell, SelectionLocation location, uint32_t card_index) {
    if (selection_location_is_cascade(location)) {
        return freecell->cascade[location - CASCADE_1].size - card_index;
    }
    return 1;
}

uint8_t freecell_get_index_from_size(
    const Freecell* freecell, SelectionLocation location, uint8_t size) {
    if (selection_location_is_cascade(location)) {
        return freecell->cascade[location - CASCADE_1].size - size;
    } else {
        return 0;
    }
}

uint8_t freecell_count_max_moves(Freecell* freecell) {
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

MoveResult freecell_validate_to_foundation(Freecell* freecell, Card card, SelectionLocation dest) {
    Suit card_suit = get_suit(card);
    Rank card_rank = get_rank(card);

    if (dest - FOUNDATION_SPADES != card_suit) {
        return MOVE_ERROR;
    }

    Card* foundation = &freecell->foundation[card_suit];
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

MoveResult freecell_validate_to_reserve(Freecell* freecell, Card card, SelectionLocation dest) {
    if (card == NONE) {
        return MOVE_ERROR;
    }

    if (freecell->reserve[dest - RESERVE_1] != NONE) {
        return MOVE_ERROR;
    }

    return MOVE_SUCCESS;
}

MoveResult freecell_validate_to_cascade_single(
    Freecell* freecell, Card card, SelectionLocation dest) {
    Cascade* cascade = &freecell->cascade[dest - CASCADE_1];

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

MoveResult freecell_validate_to_cascade(Freecell* freecell, Move move) {
    int max_moves = freecell_count_max_moves(freecell);

    if (freecell->cascade[move.to].size == 0) {
        max_moves /= 2;
    }

    if (move.size > max_moves) {
        return MOVE_ERROR_INSUFFICIENT_FREECELLS;
    }

    Cascade* from_cascade = &freecell->cascade[move.from];
    Cascade* to_cascade = &freecell->cascade[move.to];

    if (move.size > from_cascade->size) {
        return MOVE_ERROR;
    }

    if (!cascade_is_stacked_properly(from_cascade, from_cascade->size - move.size)) {
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

MoveResult freecell_validate_move(Freecell* freecell, Move move) {
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
        return freecell_validate_to_foundation(freecell, card, move.to);
    } else if (from_reserve && to_reserve) {
        Card card = freecell->reserve[move.from - RESERVE_1];
        return freecell_validate_to_reserve(freecell, card, move.to);
    } else if (from_reserve && to_cascade) {
        Card card = freecell->reserve[move.from - RESERVE_1];
        return freecell_validate_to_cascade_single(freecell, card, move.to);
    } else if (from_cascade && to_foundation && move.size == 1) {
        Cascade* cascade = &freecell->cascade[move.from - CASCADE_1];
        Card card = cascade->cards[cascade->size - move.size];
        return freecell_validate_to_foundation(freecell, card, move.to);
    } else if (from_cascade && to_reserve && move.size == 1) {
        Cascade* cascade = &freecell->cascade[move.from - CASCADE_1];
        Card card = cascade->cards[cascade->size - move.size];
        return freecell_validate_to_reserve(freecell, card, move.to);
    } else if (from_cascade && to_cascade) {
        return freecell_validate_to_cascade(freecell, move);
    }

    return MOVE_ERROR;
}

void freecell_move_to_foundation(Freecell* freecell, Card card, SelectionLocation dest) {
    Card* foundation = &freecell->foundation[get_suit(card)];
    *foundation = card;
}

void freecell_move_to_reserve(Freecell* freecell, Card card, SelectionLocation dest) {
    freecell->reserve[dest - RESERVE_1] = card;
}

void freecell_move_to_cascade_single(Freecell* freecell, Card card, SelectionLocation dest) {
    Cascade* cascade = &freecell->cascade[dest - CASCADE_1];
    cascade_push(cascade, card);
}

void freecell_move_to_cascade(Freecell* freecell, Move move) {
    Cascade* from_cascade = &freecell->cascade[move.from];
    Cascade* to_cascade = &freecell->cascade[move.to];

    memcpy(to_cascade->cards + to_cascade->size,
        from_cascade->cards + from_cascade->size - move.size, move.size);
    from_cascade->size -= move.size;
    to_cascade->size += move.size;
}

void freecell_move(Freecell* freecell, Move move) {
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
        freecell->foundation[move.from - FOUNDATION_SPADES]
            = get_rank(card) == ACE ? NONE : (card - 1);
    } else if (from_foundation && to_cascade) {
        Card card = freecell->foundation[move.from - FOUNDATION_SPADES];
        freecell_move_to_cascade_single(freecell, card, move.to);
        freecell->foundation[move.from - FOUNDATION_SPADES]
            = get_rank(card) == ACE ? NONE : (card - 1);
    } else if (from_reserve && to_foundation) {
        Card card = freecell->reserve[move.from - RESERVE_1];
        freecell_move_to_foundation(freecell, card, move.to);
        freecell->reserve[move.from - RESERVE_1] = NONE;
    } else if (from_reserve && to_reserve) {
        Card card = freecell->reserve[move.from - RESERVE_1];
        freecell_move_to_reserve(freecell, card, move.to);
        freecell->reserve[move.from - RESERVE_1] = NONE;
    } else if (from_reserve && to_cascade) {
        Card card = freecell->reserve[move.from - RESERVE_1];
        freecell_move_to_cascade_single(freecell, card, move.to);
        freecell->reserve[move.from - RESERVE_1] = NONE;
    } else if (from_cascade && to_foundation) {
        Cascade* cascade = &freecell->cascade[move.from - CASCADE_1];
        Card card = cascade->cards[cascade->size - move.size];
        freecell_move_to_foundation(freecell, card, move.to);
        cascade_pop(cascade);
    } else if (from_cascade && to_reserve) {
        Cascade* cascade = &freecell->cascade[move.from - CASCADE_1];
        Card card = cascade->cards[cascade->size - move.size];
        freecell_move_to_reserve(freecell, card, move.to);
        cascade_pop(cascade);
    } else if (from_cascade && to_cascade) {
        freecell_move_to_cascade(freecell, move);
    }
}
