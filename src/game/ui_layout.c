#include "game/ui_layout.h"

#include "core/aalloc.h"
#include "core/game.h"
#include "core/vector.h"
#include "game/constants.h"
#include "game/ui_element.h"
#include "game/ui_sprites.h"
#include "game/ui_state.h"
#include "game/world.h"

#include "utils.h"

static Rect empty_hitbox(void) {
    Rect hitbox = { 0 };
    hitbox.x = -INFINITY;
    hitbox.y = -INFINITY;
    hitbox.width = 0.0f;
    hitbox.height = 0.0f;
    return hitbox;
}

bool ui_find_in_layout(
    Vector* ui_elements,
    SelectionLocation location,
    uint32_t card_index,
    UIElement* dest,
    size_t* dest_idx
) {
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

static void ui_push_freecells(Vector* vec, World* world) {
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

static void ui_push_foundation(Vector* vec, World* world) {
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

        // pulse foundation on game over
        double t = time_millis() / 1000.0;
        double v = 0.70 + 0.15 * pow(sin(2 * t), 2);
        if (freecell_game_over(&world->game.freecell)
            && world->animation_system.ui_animations.size == 0) {
            sprite.color = (Color) {
                .r = v * 0.9,
                .g = v,
                .b = 0.55f,
                .a = 1.0f,
            };
        }

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

static void ui_push_cascade(Vector* vec, World* world, int cascade_index, int x_offset) {
    Freecell* freecell = &world->game.freecell;
    Cascade* cascade = &freecell->cascade[cascade_index];
    Sprite* deck = world->deck;

    const int MARGIN_Y = deck[NONE].height * 1.7f;
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

static void ui_push_cascades(Vector* vec, World* world) {
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

static APtr format_game_info(uint32_t seed, double seconds, uint32_t moves) {
    int hrs = (int)(seconds / 3600);
    int mins = (int)((seconds - hrs * 3600) / 60);
    int secs = (int)(seconds) % 60;

    int needed = snprintf(
        NULL,
        0,
        "# %u\n%s%02d:%02d:%02d\n%s%u",
        moves,
        ICON_CLOCK,
        hrs,
        mins,
        secs,
        ICON_GAME,
        seed
    );
    APtr buf = aalloc(needed + 1);
    snprintf(
        aptr(buf),
        needed + 1,
        "# %u\n%s%02d:%02d:%02d\n%s%u",
        moves,
        ICON_CLOCK,
        hrs,
        mins,
        secs,
        ICON_GAME,
        seed
    );
    return buf;
}

static void text_compute_size(
    const char* text,
    float glyph_width,
    float font_scaling,
    float line_height_scaling,
    float character_spacing_scaling,
    float* width,
    float* height
) {
    // assumes font width and height are the same for all characters
    // this is true for the fonts used in this game
    float font_size = glyph_width * font_scaling;

    float char_spacing = font_size / 2.0f * character_spacing_scaling;
    float line_height = font_size * line_height_scaling;

    float offset_x = 0;
    float offset_y = 0;

    float max_width = 0;

    size_t string_len = strlen(text);
    uint32_t num_lines = string_len == 0 ? 0 : 1;
    for (size_t i = 0; i < string_len; i++) {
        uint8_t c = text[i];
        if (c == '\n') {
            offset_x = 0;
            num_lines++;
        } else if (c == '\t') {
            // one tab should be equal to 4 spaces :P
            offset_x += char_spacing * 4;
        } else {
            offset_x += char_spacing;
        }
        max_width = max(max_width, offset_x);
    }

    float max_height = num_lines * line_height;

    if (width)
        *width = max_width;
    if (height)
        *height = max_height;
}

static void ui_push_shortcuts(Vector* vec, World* world) {
    const char shortcuts_str[] = "      N     - New game\n"
                                 "      Q     - Quit\n"
                                 "      F     - Toogle full screen\n"
                                 "Right click - Quick move\n"
                                 "   Escape   - Undo last move";
    APtr shortcuts = aalloc(sizeof(shortcuts_str));
    strcpy(aptr(shortcuts), shortcuts_str);

    float width, height;
    text_compute_size(
        shortcuts_str,
        world->characters[' '].height,
        0.8f,
        1.0f,
        1.0f,
        &width,
        &height
    );
    vec_push_back(vec, &(UIElement) {
        .type = UI_TEXT,
        .sprite = (Sprite) {
            .x = VIRTUAL_WIDTH - width,
            .y = VIRTUAL_HEIGHT - height,
            .color = (Color) {
                .r = 0.85,
                .g = 0.55,
                .b  = 0.55,
                .a = 1.0f,
            }
        }, 
        .hitbox = empty_hitbox(),
        .meta.text = {
            .text = shortcuts,
            .font_scaling = 0.8f,
            .line_height_scaling = 1.0f,
            .character_spacing_scaling = 1.0f,
        },
    });
}

static void ui_push_instructions(Vector* vec, World* world) {
    const char instructions_str[] = "* Move cards to four foundations in order\n"
                                    "* Use freecells to store any single card\n"
                                    "* Descending, alternating stacks can move\n"
                                    "* Stacks can move only if freecells are sufficient";

    APtr instructions = aalloc(sizeof(instructions_str));
    strcpy(aptr(instructions), instructions_str);

    float width, height;
    text_compute_size(
        aptr(instructions),
        world->characters[' '].height,
        0.8f,
        1.0f,
        1.0f,
        &width,
        &height
    );

    vec_push_back(vec, &(UIElement) {
        .type = UI_TEXT,
        .sprite = (Sprite) {
            .x = VIRTUAL_WIDTH / 2.0f - width/ 2.0f,
            .y = VIRTUAL_HEIGHT - height,
            .color = (Color) {
                .r = 0.85,
                .g = 0.55,
                .b  = 0.55,
                .a = 1.0f,
            }
        }, 
        .hitbox = empty_hitbox(),
        .meta.text = {
            .text = instructions,
            .font_scaling = 0.8f,
            .line_height_scaling = 1.0f,
            .character_spacing_scaling = 1.0f,
        },
    });
}

static void ui_push_game_info(Vector* vec, World* world) {
    APtr game_info
        = format_game_info(world->game.freecell.seed, world->game.clock, world->game.move_count);

    float width, height;
    text_compute_size(
        aptr(game_info),
        world->characters[' '].height,
        1.0f,
        1.3f,
        1.0f,
        &width,
        &height
    );

    Color game_info_color = {
        .r = 0.95f,
        .g = 0.65,
        .b = 0.65f,
        .a = 1.0f,
    };

    if (freecell_game_over(&world->game.freecell)) {
        game_info_color.r = 0.55f;
        game_info_color.g = 0.75f;
        game_info_color.b = 0.55f;
    }

    vec_push_back(vec, &(UIElement) {
        .type = UI_TEXT,
        .sprite = (Sprite) {
            .x = 30.0f,
            .y = VIRTUAL_HEIGHT - height + 10.0f,
            .color = game_info_color
        }, 
        .hitbox = empty_hitbox(),
        .meta.text = {
            .text = game_info,
            .font_scaling = 1.0f,
            .line_height_scaling = 1.3f,
            .character_spacing_scaling = 1.0f,
        },
    });
}

static void ui_push_game_over_text(Vector* vec, World* world) {
    if (!freecell_game_over(&world->game.freecell)) {
        return;
    }

    const char win_str[] = "You Won!";
    APtr win = aalloc(sizeof(win_str));
    strcpy(aptr(win), win_str);

    float width, height;
    text_compute_size("You Won!", world->characters[' '].height, 2.0f, 1.0f, 1.0f, &width, &height);
    vec_push_back(vec, &(UIElement) {
            .type = UI_TEXT,
            .sprite = (Sprite) {
                .x = VIRTUAL_WIDTH / 2.0f - width / 2.0,
                .y = VIRTUAL_HEIGHT / 4.0f - height / 2.0,
                .color = (Color) {
                    .r = 0.55f,
                    .g = 0.85f,
                    .b  = 0.55f,
                    .a = 1.0f,
                }
            }, 
            .hitbox = empty_hitbox(),
            .meta.text = {
                .text = win,
                .font_scaling = 2.0f,
                .line_height_scaling = 1.0f,
                .character_spacing_scaling = 1.0f,
            },
        });
}

static void ui_push_display(Vector* vec, World* world) {
    ui_push_shortcuts(vec, world);
    ui_push_instructions(vec, world);
    ui_push_game_info(vec, world);
    ui_push_game_over_text(vec, world);
}

static void ui_push_buttons(Vector* vec, World* world) {
    const float CARD_HEIGHT = world->deck[NONE].height;
    const float MARGIN_Y = 30.0f + CARD_HEIGHT / 2.0f;

    const float GAP = 25.0f;
    const float TOTAL_BUTTON_WIDTH = world->button_new_game.width + world->button_undo.width
        + world->button_sound.width + 2 * GAP;

    const char new_game_id_str[] = "new";
    APtr new_game_id = aalloc(sizeof(new_game_id_str));
    strcpy(aptr(new_game_id), new_game_id_str);

    Sprite new_game = world->button_new_game;
    new_game.x = VIRTUAL_WIDTH / 2.0f - TOTAL_BUTTON_WIDTH / 2.0f + new_game.width / 2.0f;
    new_game.y = MARGIN_Y;

    vec_push_back(vec, &(UIElement) {
        .type = UI_BUTTON,
        .sprite = new_game, 
        .hitbox = compute_hitbox(&new_game),
        .meta.button = {
            .id = new_game_id,
        },
    });

    const char undo_id_str[] = "undo";
    APtr undo_id = aalloc(sizeof(undo_id_str));
    strcpy(aptr(undo_id), undo_id_str);

    Sprite undo = world->button_undo;
    undo.x = new_game.x + new_game.width / 2.0f + undo.width / 2.0f + GAP;
    undo.y = MARGIN_Y;

    vec_push_back(vec, &(UIElement) {
        .type = UI_BUTTON,
        .sprite = undo, 
        .hitbox = compute_hitbox(&undo),
        .meta.button = {
            .id = undo_id,
        },
    });

    const char sound_id_str[] = "sound";
    APtr sound_id = aalloc(sizeof(sound_id_str));
    strcpy(aptr(sound_id), sound_id_str);

    Sprite sound = world->button_sound;
    sound.x = undo.x + undo.width / 2.0f + sound.width / 2.0f + GAP;
    sound.y = MARGIN_Y;

    if (world->sound_enabled) {
        sound.color = (Color) { 0.55f, 0.55f, 0.85f, 1.0f };
    } else {
        sound.color = (Color) { 0.5f, 0.5f, 0.5f, 1.0f };
    }

    vec_push_back(vec, &(UIElement) {
        .type = UI_BUTTON,
        .sprite = sound, 
        .hitbox = compute_hitbox(&sound),
        .meta.button = {
            .id = sound_id,
        },
    });
}

void ui_push_world(Vector* vec, World* world) {
    ui_push_display(vec, world);
    ui_push_buttons(vec, world);
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
