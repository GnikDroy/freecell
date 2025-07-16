#include "game/ui_layout.h"

#include "core/game.h"
#include "core/vector.h"
#include "game/constants.h"
#include "game/ui_element.h"
#include "game/ui_state.h"
#include "game/world.h"

#include "utils.h"
#include <stdlib.h>

Rect compute_hitbox(Sprite* sprite) {
    Rect hitbox = {
        .height = sprite->height,
        .width = sprite->width,
        .x = sprite->x,
        .y = sprite->y,
    };

    return hitbox;
}

static Rect empty_hitbox(void) {
    Rect hitbox = { 0 };
    hitbox.x = -INFINITY;
    hitbox.y = -INFINITY;
    hitbox.width = 0.0f;
    hitbox.height = 0.0f;
    return hitbox;
}

bool ui_find_in_layout(Vector* ui_elements, SelectionLocation location, uint32_t card_index,
    UIElement* dest, size_t* dest_idx) {
    for (size_t i = 0; i < ui_elements->size; i++) {
        vec_get_as(UIElement, element, ui_elements, i);
        if (element.type == UI_CARD && element.meta.card.selection_location == location
            && element.meta.card.card_index == card_index) {
            if (dest != NULL) {
                *dest = element;
            }

            if (dest_idx != NULL) {
                *dest_idx = i;
            }
            return true;
        }
    }

    return false;
}

void ui_push_freecells(Vector* vec, World* world) {
    Sprite* deck = world->deck;
    Freecell* freecell = &world->game.freecell;

    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 25;

    for (int i = 0; i < 4; i++) {
        // PLACEHOLDER WHEN NO CARD IS PRESENT
        Sprite none_card = deck[NONE];
        none_card.x = (none_card.width + GAP) * i + none_card.width / 2.f + MARGIN_X;
        none_card.y = none_card.height / 2.f + MARGIN_Y;
        none_card.z = 0.0f;
        none_card.color.a = 0.5f;
        vec_push_back(vec, &(UIElement) {
            .type = UI_CARD_PLACEHOLDER,
            .sprite = none_card,
            .hitbox = empty_hitbox(),
            .meta.card = {
                .card = NONE,
                .selection_location = RESERVE_1 + i,
                .card_index = 0,
                .state = CARD_UI_STATE_NORMAL,
            },
        });

        // Actual card
        Card card = freecell->reserve[i];
        Sprite sprite = deck[card];

        if (card == NONE) {
            sprite.color.a = 0.0f;
        }

        sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
        sprite.y = sprite.height / 2.f + MARGIN_Y;
        sprite.z = 0.0f;

        UIElement ui_element = {
            .type = UI_CARD,
            .sprite = sprite,
            .hitbox = compute_hitbox(&sprite),
            .meta.card = {
                .card = card,
                .selection_location = RESERVE_1 + i,
                .card_index = 0,
                .state = CARD_UI_STATE_NORMAL,
            },
        };

        vec_push_back(vec, &ui_element);
    }
}

void ui_push_foundation(Vector* vec, World* world) {
    Sprite* deck = world->deck;
    Freecell* freecell = &world->game.freecell;

    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 25;

    for (int i = 0; i < 4; i++) {
        // Placeholder when no card is present
        Card none_card = freecell->foundation[i];
        float none_alpha = 1.0f;
        if (none_card == NONE || get_rank(none_card) == ACE) {
            none_card = ACE_SPADES + 13 * i;
            none_alpha = 0.3f;
        } else {
            none_card -= 1;
        }
        Sprite none_sprite = deck[none_card];
        none_sprite.x = (none_sprite.width + GAP) * i + none_sprite.width / 2.f + MARGIN_X;
        none_sprite.x = VIRTUAL_WIDTH - none_sprite.x;
        none_sprite.y = none_sprite.height / 2.f + MARGIN_Y;
        none_sprite.z = 0.0f;
        none_sprite.color.a = none_alpha;
        vec_push_back(vec, &(UIElement) {
            .type = UI_CARD_PLACEHOLDER,
            .sprite = none_sprite,
            .hitbox = empty_hitbox(),
            .meta.card =
                {
                    .card = none_card,
                    .selection_location = FOUNDATION_SPADES + i,
                    .card_index = 0,
                    .state = CARD_UI_STATE_NORMAL,
                },
        });

        // Actual card
        Card card = freecell->foundation[i];
        Sprite sprite = deck[card];
        if (card == NONE) {
            sprite.color.a = 0.0f;
        }
        sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
        sprite.x = VIRTUAL_WIDTH - sprite.x;
        sprite.y = sprite.height / 2.f + MARGIN_Y;
        sprite.z = 0.0f;

        UIElement ui_element = {
            .type = UI_CARD,
            .sprite = sprite,
            .hitbox = compute_hitbox(&sprite),
            .meta.card =
                {
                    .card = card,
                    .selection_location = FOUNDATION_SPADES + i,
                    .card_index = 0,
                    .state = CARD_UI_STATE_NORMAL,
                },
        };

        vec_push_back(vec, &ui_element);
    }
}

void ui_push_cascade(Vector* vec, World* world, int cascade_index, int x_offset) {
    Freecell* freecell = &world->game.freecell;
    Cascade* cascade = &freecell->cascade[cascade_index];
    Sprite* deck = world->deck;

    const int MARGIN_Y = deck[NONE].height * 2;
    const int OVERLAP = floor(deck[NONE].height * 0.2);

    // If the cascade is empty, we add a UIElement with a placeholder card.
    // So that cards can be placed here
    // This card can be interacted with, therefore it has a type UI_CARD
    if (cascade->size == 0) {
        Card card = NONE;
        Sprite sprite = deck[card];
        sprite.x = x_offset;
        sprite.y = sprite.height / 2.f + MARGIN_Y;
        sprite.z = 0.0f;

        sprite.color.a = 0.5f;

        UIElement ui_element = {
        .type = UI_CARD,
        .sprite = sprite,
        .hitbox = compute_hitbox(&sprite),
        .meta.card =
            {
                .card = card,
                .selection_location = CASCADE_1 + cascade_index,
                .card_index = 0,
                .state = CARD_UI_STATE_NORMAL,
            },
    };

        vec_push_back(vec, &ui_element);
    }

    for (int j = 0; j < cascade->size; j++) {
        Card card = cascade->cards[j];
        Sprite sprite = deck[card];
        sprite.x = x_offset;
        sprite.y = sprite.height / 2.f + MARGIN_Y + OVERLAP * j;
        sprite.z = 0.0f;

        UIElement ui_element = {
        .type = UI_CARD,
        .sprite = sprite,
        .hitbox = compute_hitbox(&sprite),
        .meta.card =
            {
                .card = card,
                .selection_location = CASCADE_1 + cascade_index,
                .card_index = j,
                .state = CARD_UI_STATE_NORMAL,
            },
    };

        vec_push_back(vec, &ui_element);
    }
}

void ui_push_cascades(Vector* vec, World* world) {
    Sprite* deck = world->deck;

    const int CASCADE_COUNT = 8;
    const int GAP = 45;

    const float total_width = CASCADE_COUNT * deck[NONE].width + (CASCADE_COUNT - 1) * GAP;
    for (int i = 0; i < CASCADE_COUNT; i++) {
        int x_offset = (VIRTUAL_WIDTH - total_width) / 2.0f + i * (deck[NONE].width + GAP)
            + deck[NONE].width / 2.f;
        ui_push_cascade(vec, world, i, x_offset);
    }
}

void ui_push_world(Vector* vec, World* world) {
    ui_push_freecells(vec, world);
    ui_push_foundation(vec, world);
    ui_push_cascades(vec, world);
}

bool ui_get_topmost_hit(Vector* ui_elements, vec2s mouse, UIElement* topmost, size_t* index) {
    if (ui_elements->size == 0) {
        return false;
    }

    for (int i = (int)ui_elements->size - 1; i >= 0; i--) {
        vec_get_as(UIElement, ui_element, ui_elements, i);

        if (point_in_rect(mouse.x, mouse.y, ui_element.hitbox)) {
            if (index != NULL) {
                *index = i;
            }

            if (topmost != NULL) {
                *topmost = ui_element;
            }
            return true;
        }
    }

    return false;
}
