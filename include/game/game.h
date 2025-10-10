#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "game/freecell.h"
#include "core/vector.h"

typedef struct Game {
    Freecell freecell;
    uint32_t seed;
    size_t move_count;
    Vector history;
    double clock;
} Game;

Game game_init(void);

void game_free(Game* game);

void game_new(Game* game);

void game_new_from_seed(Game* game, uint32_t seed);

bool game_can_move_from(Game* game, SelectionLocation from, uint32_t card_index);

MoveResult game_validate_move(Game* game, Move move);

MoveResult game_move(Game* game, Move move);

MoveResult game_undo(Game* game);
