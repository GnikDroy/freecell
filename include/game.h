#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "freecell.h"
#include "vector.h"

typedef struct Game {
  Freecell freecell;
  size_t move_count;
  Vector history;
} Game;

Game game_init(void);

void game_free(Game *game);

void game_new(Game *game);

bool game_can_move_from(Game *game, SelectionLocation from,
                        uint32_t card_index);

MoveResult game_move(Game *game, Move move);

void game_undo(Game *game);
