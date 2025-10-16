#include "game/controller.h"

#include "core/aalloc.h"
#include "core/vector.h"

#include "game/animation.h"
#include "game/game.h"
#include "game/input.h"
#include "game/ui_element.h"
#include "game/ui_state.h"
#include "platform/window.h"

#include "game/constants.h"
#include "game/render_system.h"
#include "game/ui_layout.h"
#include "game/world.h"

#include "utils.h"

static void controller_play_card_move_sound(World* world) {
    if (world->sound_enabled) {
        ma_sound_stop(&world->card_move_sound);
        ma_sound_start(&world->card_move_sound);
    }
}

static void controller_animate_game_over(World* world) {
    AnimationSystem* animation_system = &world->animation_system;

    // Don't add too many animations if there are already animations running
    if (world->animation_system.ui_animations.size > 2) {
        return;
    } else if (world->animation_system.ui_animations.size > 0) {
        float progress_threshold = 0.5;
        vec_get_as(
            UIElementAnimation,
            anim,
            &world->animation_system.ui_animations,
            world->animation_system.ui_animations.size - 1
        );
        if (anim.elapsed / anim.duration < progress_threshold) {
            return;
        }
    }

    // nothing left in foundation
    bool found = false;
    for (int i = 0; i < 4; i++) {
        if (world->game.freecell.foundation[0] != NONE) {
            found = true;
        }
    }
    if (!found)
        return;

    Freecell* freecell = &world->game.freecell;

    // find largest ranked foundation.
    int idx = 0;
    int largest_rank = -1;
    for (int i = 3; i >= 0; i--) {
        if (freecell->foundation[i] != NONE && get_rank(freecell->foundation[i]) > largest_rank) {
            largest_rank = get_rank(freecell->foundation[i]);
            idx = i;
        }
    }

    UIElement foundation_item;
    if (ui_find_in_layout(
            &world->ui_elements,
            FOUNDATION_SPADES + idx,
            0,
            &foundation_item,
            NULL
        )) {
        Card card = world->game.freecell.foundation[idx];
        Rank rank = get_rank(card);
        Card new_card = (rank == ACE) ? NONE : get_card(rank - 1, get_suit(card));
        world->game.freecell.foundation[idx] = new_card;

        UIElement* elem = &foundation_item;
        float duration = 4.0f;

        if (new_card != NONE) {
            UIElementAnimation static_animation = { 0 };
            static_animation.duration = duration;
            static_animation.behaviour = ANIMATION_DELETE_ON_FINISH;
            Sprite sprite = world->deck[new_card];
            sprite.x = elem->sprite.x;
            sprite.y = elem->sprite.y;
            sprite.z = 0.0f;
            static_animation.from = (UIElement) {
                .type = UI_CARD,
                .sprite = sprite,
                .hitbox = compute_hitbox(&sprite),
                .meta.card = {
                    .card = card,
                    .selection_location = elem->meta.card.selection_location,
                    .card_index = 0,
                    .state = CARD_UI_STATE_NORMAL,
                },
            };

            static_animation.to = static_animation.from;
            vec_push_back(&animation_system->ui_animations, &static_animation);
        }

        // actual animation
        int x_offset = random_uniform() * VIRTUAL_WIDTH / 5.0f;
        int y_offset = 2.0f * VIRTUAL_HEIGHT / 3.0f + random_uniform() * (VIRTUAL_HEIGHT / 3.0f);

        Sprite sprite = world->deck[card];
        sprite.x = elem->sprite.x;
        sprite.y = elem->sprite.y;
        sprite.z = 0.0f;

        UIElementAnimation animation = { 0 };
        animation.duration = duration;
        animation.behaviour = ANIMATION_DELETE_ON_FINISH;
        animation.from = (UIElement) {
                .type = UI_CARD,
                .sprite = sprite,
                .hitbox = compute_hitbox(&sprite),
                .meta.card = {
                    .card = card,
                    .selection_location = elem->meta.card.selection_location,
                    .card_index = 0,
                    .state = CARD_UI_STATE_NORMAL,
                },
            };

        Sprite to = sprite;
        to.color.r = 0.4 + random_uniform() * 0.6;
        to.color.g = 0.4 + random_uniform() * 0.6;
        to.color.b = 0.4 + random_uniform() * 0.6;
        to.color.a = 0.0f;
        to.x -= x_offset;
        to.y += y_offset;
        to.rotation = 2 * M_PI * (random_uniform() > 0.5f ? 1.0f : -1.0f) + M_PI * random_uniform();
        to.width *= 2;
        to.height *= 2;

        animation.to = (UIElement) {
                .type = UI_CARD,
                .sprite = to,
                .hitbox = compute_hitbox(&to),
                .meta.card = {
                    .card = card,
                    .selection_location = elem->meta.card.selection_location,
                    .card_index = 0,
                    .state = CARD_UI_STATE_NORMAL,
                },
            };

        vec_push_back(&animation_system->ui_animations, &animation);
    }
}

static void controller_animate_new_game(World* world) {
    // Before new game ui_elements
    AnimationSystem* animation_system = &world->animation_system;
    animation_system->ui_animations.size = 0; // clear all other animations.

    for (int i = 0; i < world->ui_elements.size; i++) {
        vec_get_as(UIElement, to, &world->ui_elements, i);
        UIElement from = to;
        to.sprite.x = VIRTUAL_WIDTH / 2.0;
        to.sprite.y = VIRTUAL_HEIGHT / 2.0;

        if (to.type == UI_CARD && to.meta.card.card != NONE) {
            vec_push_back(
                &animation_system->ui_animations,
                &(UIElementAnimation) {
                    .from = from,
                    .to = to,
                    .elapsed = 0.0f,
                    .duration = 0.3f,
                    .behaviour = ANIMATION_DELETE_ON_FINISH,
                }
            );
        }
    }

    render_world(world);

    // After new game ui_elements
    for (int i = 0; i < world->ui_elements.size; i++) {
        vec_get_as(UIElement, to, &world->ui_elements, i);
        UIElement from = to;
        from.sprite.x = VIRTUAL_WIDTH / 2.0;
        from.sprite.y = VIRTUAL_HEIGHT / 2.0;

        if (to.type == UI_CARD) {
            vec_push_back(
                &animation_system->ui_animations,
                &(UIElementAnimation) {
                    .from = from,
                    .to = to,
                    .elapsed = -0.3f,
                    .duration = 0.3f,
                    .behaviour = ANIMATION_DELETE_ON_FINISH,
                }
            );
        }
    }
}

static MoveResult controller_animated_move(World* world, Move move, float duration) {
    Controller* controller = &world->controller;
    AnimationSystem* animation_system = &world->animation_system;

    MoveResult result = game_validate_move(&world->game, move);
    if (result != MOVE_SUCCESS) {
        return result;
    }

    uint8_t from_move_index
        = freecell_get_index_from_size(&world->game.freecell, move.from, move.size);

    UIElement from, to;
    if (ui_find_in_layout(&world->ui_elements, move.from, from_move_index, &from, NULL)) {
        // We don't need to support cascade animation for stacks
        uint8_t to_move_index = 0;
        if (ui_find_in_layout(&world->ui_elements, move.to, 0, &to, NULL)) {
            to.sprite.color.a = from.sprite.color.a;

            vec_push_back(
                &animation_system->ui_animations,
                &(UIElementAnimation) {
                    .from = from,
                    .to = to,
                    .elapsed = 0.0f,
                    .duration = duration,
                    .behaviour = ANIMATION_DELETE_ON_FINISH,
                }
            );
        }
    }

    game_move(&world->game, move);
    controller_play_card_move_sound(world);
    return result;
}

static void controller_autocomplete_game(World* world) {
    if (freecell_game_over(&world->game.freecell)
        || !freecell_is_trivially_solved(&world->game.freecell)) {
        return;
    }

    // better UX if i allow more than one animation
    // if the earlier is beyond threshold
    if (world->animation_system.ui_animations.size > 2) {
        return;
    } else if (world->animation_system.ui_animations.size > 0) {
        float progress_threshold = 0.5;
        vec_get_as(
            UIElementAnimation,
            anim,
            &world->animation_system.ui_animations,
            world->animation_system.ui_animations.size - 1
        );
        if (anim.elapsed / anim.duration < progress_threshold) {
            return;
        }
    }

    // find the smallest card in the cascade (or reserve)
    Freecell* freecell = &world->game.freecell;

    SelectionLocation smallest_location = (uint8_t)-1;
    Suit smallest_suit = (uint8_t)-1;
    Rank smallest_rank = (uint8_t)-1;
    for (int i = 0; i < 4; i++) {
        if (freecell->reserve[i] != NONE) {
            Card card = freecell->reserve[i];
            Suit suit = get_suit(card);
            Rank rank = get_rank(card);

            // Suits are ranked in descending order in the enum
            if (rank < smallest_rank || (rank == smallest_rank && suit > smallest_suit)) {
                smallest_suit = suit;
                smallest_rank = rank;
                smallest_location = RESERVE_1 + i;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        Cascade* cascade = &freecell->cascade[i];
        size_t cascade_size = cascade->size;
        if (cascade_size > 0) {
            Card card = cascade->cards[cascade->size - 1];
            Suit suit = get_suit(card);
            Rank rank = get_rank(card);

            // Suits are ranked in descending order in the enum
            if (rank < smallest_rank || (rank == smallest_rank && suit > smallest_suit)) {
                smallest_suit = suit;
                smallest_rank = rank;
                smallest_location = CASCADE_1 + i;
            }
        }
    }

    Card smallest_card = get_card(smallest_rank, smallest_suit);

    // Now we construct the move to the foundation
    Move move = {
        .from = smallest_location,
        .to = get_suit(smallest_card) + FOUNDATION_SPADES,
        .size = 1,
    };

    controller_animated_move(world, move, 0.35f);
}

void controller_update(World* world, double dt) {
    world->controller.screen_needs_update = false;
    controller_handle_inputs(world);

    Controller* controller = &world->controller;
    if (!freecell_game_over(&world->game.freecell)) {
        world->game.clock += dt;
    }

    controller_update_drag(world);
    animation_system_update(&world->animation_system, controller, dt);
    render_world(world);
    controller_autocomplete_game(world);

    if (freecell_game_over(&world->game.freecell)) {
        controller_animate_game_over(world);
    }
}

void controller_click(World* world) {
    vec2s mouse = world->controller.mouse;

    UIElement ui_element;
    size_t index;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &ui_element, &index)) {
        if (ui_element.type == UI_BUTTON) {
            // handle button click
            const char* id = aptr(ui_element.meta.button.id);

            if (strcmp(id, "new") == 0) {
                controller_new_game(world);
            } else if (strcmp(id, "undo") == 0) {
                controller_undo(world);
            } else if (strcmp(id, "sound") == 0) {
                world->sound_enabled = !world->sound_enabled;
            }
        }
    }
}

void controller_start_drag(World* world) {
    vec2s mouse = world->controller.mouse;

    UIElement ui_element;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &ui_element, NULL)) {
        if (!ui_is_element_draggable(world, &ui_element) || ui_element.type != UI_CARD) {
            return;
        }

        UIDragState* drag_state = &world->controller.drag_state;
        drag_state->dragging = true;
        drag_state->drag_offset.x = mouse.x;
        drag_state->drag_offset.y = mouse.y;
        drag_state->card_location = ui_element.meta.card.selection_location;
        drag_state->card_index = ui_element.meta.card.card_index;
    }
}

void controller_update_drag(World* world) {
    if (freecell_game_over(&world->game.freecell)
        || world->animation_system.ui_animations.size > 0) {
        world->controller.drag_state.dragging = false;
    }
}

void controller_end_drag(World* world) {
    UIDragState* drag_state = &world->controller.drag_state;

    // handle ui element drop
    Vector* ui_elements = &world->ui_elements;
    UIElement dest;
    if (drag_state->dragging
        && ui_get_topmost_hit(ui_elements, world->controller.mouse, &dest, NULL)
        && dest.type == UI_CARD) {
        controller_handle_card_drop(
            &dest,
            drag_state->card_location,
            drag_state->card_index,
            world
        );
    }

    // Update drag state and queue layout update
    drag_state->dragging = false;
    drag_state->drag_offset.x = 0;
    drag_state->drag_offset.y = 0;
    drag_state->card_location = -1;
    drag_state->card_index = 0;
}

bool controller_handle_card_drop(
    UIElement* dest,
    SelectionLocation from_location,
    uint8_t card_index,
    World* world
) {

    if (freecell_game_over(&world->game.freecell)) {
        return false;
    }

    if (dest->type != UI_CARD) {
        return false;
    }

    SelectionLocation dest_location = dest->meta.card.selection_location;

    // if destination is ANY foundation,
    // dest foundation will be autocorrected to the suit of the source
    if (selection_location_is_foundation(dest_location)) {
        Card from_card = freecell_get_card(&world->game.freecell, from_location, card_index);
        dest_location = FOUNDATION_SPADES + get_suit(from_card);
    }

    uint8_t size
        = freecell_count_cards_from_index(&world->game.freecell, from_location, card_index);

    MoveResult result = game_move(
        &world->game,
        (Move) {
            .from = from_location,
            .to = dest_location,
            .size = size,
        }
    );

    if (result == MOVE_SUCCESS) {
        controller_play_card_move_sound(world);
    }

    return result == MOVE_SUCCESS;
}

void controller_smart_move(World* world) {
    Controller* controller = &world->controller;
    vec2s mouse = controller->mouse;

    AnimationSystem* animation_system = &world->animation_system;

    UIElement topmost;
    size_t index;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &topmost, &index)) {
        if (animation_system_is_animated(animation_system, &topmost)) {
            // if we are clicking on an animated element, do nothing
            return;
        }

        if (topmost.type == UI_CARD) {
            Card card = topmost.meta.card.card;
            SelectionLocation location = topmost.meta.card.selection_location;

            if (card == NONE) {
                return;
            }

            uint8_t size = freecell_count_cards_from_index(
                &world->game.freecell,
                location,
                topmost.meta.card.card_index
            );

            // foundation destination is automatically corrected to the suit of the card
            SelectionLocation foundation_destination = get_suit(card) + FOUNDATION_SPADES;

            // if can move to foundation, do it
            if (freecell_validate_to_foundation(&world->game.freecell, card, foundation_destination)
                == MOVE_SUCCESS) {
                SelectionLocation to_location = FOUNDATION_SPADES + get_suit(card);

                controller_animated_move(
                    world,
                    (Move) {
                        .from = location,
                        .to = to_location,
                        .size = size,
                    },
                    0.3f
                );
                return;
            }

            // if can move to any reserve, do it
            for (uint8_t i = 0; i < 4; i++) {
                if (freecell_validate_to_reserve(&world->game.freecell, card, RESERVE_1 + i)
                    == MOVE_SUCCESS) {
                    controller_animated_move(
                        world,
                        (Move) {
                            .from = location,
                            .to = RESERVE_1 + i,
                            .size = size,
                        },
                        0.3f
                    );
                    return;
                }
            }
        }
    }
}

void controller_on_framebuffer_resize(World* world, int width, int height) {
    float aspectVirtual = (float)VIRTUAL_WIDTH / (float)VIRTUAL_HEIGHT;
    float aspectWindow = (float)width / (float)height;

    int viewportWidth, viewportHeight;
    int viewportX, viewportY;

    if (aspectWindow > aspectVirtual) {
        viewportHeight = height;
        viewportWidth = (int)((float)viewportHeight * aspectVirtual);
    } else {
        viewportWidth = width;
        viewportHeight = (int)((float)viewportWidth / aspectVirtual);
    }

    viewportX = (width - viewportWidth) / 2;
    viewportY = (height - viewportHeight) / 2;

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void controller_on_cursor_position(World* world, double x, double y) {
    Controller* controller = &world->controller;

    controller->mouse_screen.x = (float)x;
    controller->mouse_screen.y = (float)y;

    int width, height;
    window_get_size(world->window, &width, &height);
    controller->mouse = screen_to_world(x, y, width, height, &world->camera);
}

void controller_undo(World* world) {
    Controller* controller = &world->controller;

    if (freecell_game_over(&world->game.freecell)
        || freecell_is_trivially_solved(&world->game.freecell)) {
        return;
    }

    if (game_undo(&world->game) == MOVE_SUCCESS) {
        world->animation_system.ui_animations.size = 0;
        controller_play_card_move_sound(world);
    }
}

void controller_new_game(World* world) {
    Controller* controller = &world->controller;
    world->animation_system.ui_animations.size = 0;
    game_new(&world->game);
    controller_animate_new_game(world);
}

void controller_new_game_with_seed(World* world, uint32_t seed) {
    Controller* controller = &world->controller;
    world->animation_system.ui_animations.size = 0;
    game_new_from_seed(&world->game, seed);
    controller_animate_new_game(world);
}

void controller_toggle_fullscreen(World* world) {
    Controller* controller = &world->controller;

    window_toggle_fullscreen(world->window);
}

void controller_toggle_help(World* world) { world->show_help = !world->show_help; }

static void controller_autocompleteable_game(World* world) {
    (void)world;
#ifdef FREECELL_DEBUG
    Controller* controller = &world->controller;

    // autocompleteable game for debug
    world->game.seed = 0;
    Freecell* freecell = &world->game.freecell;

    freecell->foundation[SPADES] = NONE;
    freecell->foundation[HEARTS] = NONE;
    freecell->foundation[DIAMONDS] = NONE;
    freecell->foundation[CLUBS] = NONE;

    for (int i = 0; i < 8; i++) {
        freecell->cascade[i].size = 0;
    }

    for (int i = 0; i < 12; i++) {
        Card card_a = get_card(QUEEN - i, i % 2 == 0 ? SPADES : DIAMONDS);
        Card card_b = get_card(QUEEN - i, i % 2 != 0 ? SPADES : DIAMONDS);
        Card card_c = get_card(QUEEN - i, CLUBS);
        Card card_d = get_card(QUEEN - i, HEARTS);
        cascade_push(&freecell->cascade[3], card_a);
        cascade_push(&freecell->cascade[4], card_b);
        cascade_push(&freecell->cascade[5], card_c);
        cascade_push(&freecell->cascade[6], card_d);
    }

    cascade_push(&freecell->cascade[0], KING_CLUBS);
    cascade_push(&freecell->cascade[0], KING_SPADES);
    freecell->reserve[0] = KING_DIAMONDS;
    freecell->reserve[1] = KING_HEARTS;
    freecell->reserve[2] = NONE;
    freecell->reserve[3] = NONE;

    world->game.history.size = 0;
    world->game.move_count = 0;
    world->game.clock = 0.0;
    world->animation_system.ui_animations.size = 0;
#endif
}

static void controller_fill_cascades(World* world) {
    (void)world;
#ifdef FREECELL_DEBUG
    Controller* controller = &world->controller;

    // make all cascades full for debugging view
    world->game.seed = 0;
    Freecell* freecell = &world->game.freecell;

    const int MAX_CASCADE_SIZE = sizeof(freecell->cascade[0].cards) / sizeof(Card);
    for (int j = 0; j < 8; j++) {
        freecell->cascade[j].size = 0;
        for (int i = 0; i < MAX_CASCADE_SIZE; i++) {
            cascade_push(&freecell->cascade[j], KING_CLUBS - i);
        }
    }

    for (int i = 0; i < 4; i++) {
        freecell->reserve[i] = NONE;
        freecell->foundation[i] = NONE;
    }

    world->game.history.size = 0;
    world->game.move_count = 0;
    world->game.clock = 0.0;
    world->animation_system.ui_animations.size = 0;
#endif
}

void controller_handle_input(InputAction ia) {
    if (ia.type == INPUT_ACTION_FRAMEBUFFER_RESIZE) {
        controller_on_framebuffer_resize(
            ia.world,
            ia.data.framebuffer_resize.width,
            ia.data.framebuffer_resize.height
        );
    } else if (ia.type == INPUT_ACTION_POINTER_MOVE) {
        controller_on_cursor_position(ia.world, ia.data.pointer_move.x, ia.data.pointer_move.y);
    } else if (ia.type == INPUT_ACTION_CLICK) {
        controller_click(ia.world);
    } else if (ia.type == INPUT_ACTION_START_DRAG) {
        controller_start_drag(ia.world);
    } else if (ia.type == INPUT_ACTION_END_DRAG) {
        controller_end_drag(ia.world);
    } else if (ia.type == INPUT_ACTION_SMART_MOVE) {
        controller_smart_move(ia.world);
    } else if (ia.type == INPUT_ACTION_UNDO) {
        controller_undo(ia.world);
    } else if (ia.type == INPUT_ACTION_NEW_GAME) {
        controller_new_game(ia.world);
    } else if (ia.type == INPUT_ACTION_NEW_GAME_WITH_SEED) {
        controller_new_game_with_seed(ia.world, ia.data.new_game_with_seed.seed);
    } else if (ia.type == INPUT_ACTION_TOGGLE_FULLSCREEN) {
        controller_toggle_fullscreen(ia.world);
    } else if (ia.type == INPUT_ACTION_TOGGLE_HELP) {
        controller_toggle_help(ia.world);
    } else if (ia.type == INPUT_ACTION_AUTOCOMPLETEABLE_GAME) {
        controller_autocompleteable_game(ia.world);
    } else if (ia.type == INPUT_ACTION_FILL_CASCADES) {
        controller_fill_cascades(ia.world);
    }
}

void controller_handle_inputs(World* world) {
    InputAction ia;
    while (input_get_input_action(world->window, world, &ia)) {
        controller_handle_input(ia);
    }
}
