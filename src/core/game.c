#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "core/game.h"

static uint32_t get_initial_seed(void) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    uint64_t seed = (uint32_t)(now.tv_sec ^ (now.tv_nsec << 21));
    seed ^= (seed >> 33);
    seed *= 0xff51afd7ed558ccdULL;
    seed ^= (seed >> 33);
    seed *= 0xc4ceb9fe1a85ec53ULL;
    seed ^= (seed >> 33);
    seed = seed % 10000000 + 1;
    return seed;
}

Game game_init(void) {
    Game game = {
        .freecell = freecell_init(get_initial_seed()),
        .move_count = 0,
        .history = vec_init(sizeof(Move)),
        .clock = 0,
    };
    return game;
}

void game_free(Game* game) { vec_free(&game->history); }

void game_new(Game* game) {
    game->freecell = freecell_init(get_initial_seed());
    game->clock = 0;
    game->move_count = 0;
    game->history.size = 0;
}

void game_new_from_seed(Game* game, uint32_t seed) {
    game->freecell = freecell_init(seed);
    game->clock = 0;
    game->move_count = 0;
    game->history.size = 0;
}

bool game_can_move_from(Game* game, SelectionLocation from, uint32_t card_index) {
    Freecell* freecell = &game->freecell;

    if (selection_location_is_foundation(from)) {
        return freecell->foundation[from - FOUNDATION_SPADES] != NONE;
    } else if (selection_location_is_reserve(from)) {
        return freecell->reserve[from - RESERVE_1] != NONE;
    } else if (selection_location_is_cascade(from)) {
        uint32_t cascade_idx = from - CASCADE_1;
        Cascade* cascade = &freecell->cascade[cascade_idx];

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

        // cascade should be stacked properly to be moved
        return cascade_is_stacked_properly(cascade, card_index);
    }

    return false;
}

MoveResult game_validate_move(Game* game, Move move) {
    return freecell_validate_move(&game->freecell, move);
}

MoveResult game_move(Game* game, Move move) {
    MoveResult result = freecell_validate_move(&game->freecell, move);
    if (result == MOVE_SUCCESS) {
        game->move_count += freecell_move_count(&game->freecell, move);
        freecell_move(&game->freecell, move);
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

MoveResult game_undo(Game* game) {
    if (game->history.size == 0) {
        return MOVE_ERROR;
    }

    vec_get_as(Move, last_move, &game->history, game->history.size - 1);

    Move reverse_move = move_get_reverse(last_move);
    freecell_move(&game->freecell, reverse_move);

    vec_pop_back(&game->history);

    // Freecell rules allow undoing moves,
    // so we increment the move count
    game->move_count += freecell_move_count(&game->freecell, last_move);

    return MOVE_SUCCESS;
}
