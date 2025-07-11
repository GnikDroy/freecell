#pragma once

#include "game.h"
#include "image.h"

typedef struct {
    Game game;
    Image spritesheet;
    float projection[16];
} World;

World world_init(void) {
    World world;
    world.game = game_init();
    world.spritesheet = image_load("assets/cards.png", 4);
    if (world.spritesheet.data == NULL) {
        fprintf(stderr, "Failed to load spritesheet: %s\n", image_get_error_msg());
        exit(EXIT_FAILURE);
    }
    return world;
}

void world_free(World *world) {
    game_free(&world->game);
    image_free(&world->spritesheet);
}
