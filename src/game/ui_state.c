#include <glad/glad.h>

#include <RGFW.h>

#include "game/game.h"
#include "game/ui_element.h"
#include "game/ui_layout.h"
#include "game/ui_state.h"
#include "game/world.h"
#include "platform/window.h"

static void ui_element_apply_style(UIElement* ui_element) {
    if (ui_element->type == UI_CARD) {
        if (ui_element->meta.card.state == CARD_UI_STATE_SELECTED) {
            ui_element->sprite.color.r *= 0.6f;
            ui_element->sprite.color.g *= 0.6f;
            ui_element->sprite.color.b *= 0.6f;
        } else if (ui_element->meta.card.state == CARD_UI_STATE_HOVERED) {
            ui_element->sprite.color.r *= 0.8f;
            ui_element->sprite.color.g *= 0.8f;
            ui_element->sprite.color.b *= 0.8f;
        } else if (ui_element->meta.card.state == CARD_UI_STATE_DROP_TARGET) {
            ui_element->sprite.color.r *= 0.8f;
            ui_element->sprite.color.g *= 1.0f;
            ui_element->sprite.color.b *= 0.8f;
        } else {
            ui_element->sprite.color.r *= 1.0f;
            ui_element->sprite.color.g *= 1.0f;
            ui_element->sprite.color.b *= 1.0f;
        }
    } else if (ui_element->type == UI_BUTTON) {
        if (ui_element->meta.button.state == BUTTON_UI_STATE_SELECTED) {
            ui_element->sprite.color.r *= 0.6f;
            ui_element->sprite.color.g *= 0.6f;
            ui_element->sprite.color.b *= 0.6f;
        } else if (ui_element->meta.button.state == BUTTON_UI_STATE_HOVERED) {
            ui_element->sprite.color.r *= 0.8f;
            ui_element->sprite.color.g *= 0.8f;
            ui_element->sprite.color.b *= 0.8f;
        } else if (ui_element->meta.button.state == BUTTON_UI_STATE_DISABLED) {
            ui_element->sprite.color.r *= 0.7f;
            ui_element->sprite.color.g *= 0.7f;
            ui_element->sprite.color.b *= 0.7f;
            ui_element->sprite.color.a *= 0.7f;
        } else {
            ui_element->sprite.color.r *= 1.0f;
            ui_element->sprite.color.g *= 1.0f;
            ui_element->sprite.color.b *= 1.0f;
        }
    }
}

static CardUIState ui_card_state_transition(
    CardUIState current,
    bool hovered,
    bool clicked,
    bool can_move_from,
    bool can_move_to,
    bool is_dragged,
    bool is_target
) {
    if (is_dragged) {
        return CARD_UI_STATE_SELECTED;
    } else if (is_target && can_move_to && !hovered) {
        return CARD_UI_STATE_DROP_TARGET;
    } else if (is_target && can_move_to && hovered) {
        return CARD_UI_STATE_HOVERED;
    } else if (!is_target && hovered && can_move_from && !is_dragged) {
        return CARD_UI_STATE_HOVERED;
    } else if (clicked && can_move_to) {
        return CARD_UI_STATE_SELECTED;
    }
    return CARD_UI_STATE_NORMAL;
}

static ButtonUIState ui_button_state_transition(
    ButtonUIState current,
    bool hovered,
    bool clicked,
    bool disabled
) {
    if (disabled) {
        return BUTTON_UI_STATE_DISABLED;
    } else if (clicked) {
        return BUTTON_UI_STATE_SELECTED;
    } else if (hovered) {
        return BUTTON_UI_STATE_HOVERED;
    } else {
        return BUTTON_UI_STATE_NORMAL;
    }
}

bool ui_is_element_draggable(World* world, UIElement* elem) {
    if (elem->type != UI_CARD)
        return false;

    SelectionLocation loc = elem->meta.card.selection_location;
    uint32_t index = elem->meta.card.card_index;

    return game_can_move_from(&world->game, loc, index);
}

bool ui_set_dragged_element_properties(UIElement* ui_element, World* world) {
    UIDragState* drag_state = &world->controller.drag_state;

    // only card elements will have properties changed by drag
    if (ui_element->type != UI_CARD) {
        return false;
    }

    // Not being dragged or not the correct card
    if (!drag_state->dragging
        || drag_state->card_location != ui_element->meta.card.selection_location
        || drag_state->card_index > ui_element->meta.card.card_index)
        return false;

    vec2s mouse = world->controller.mouse;

    float offset_x = mouse.x - drag_state->drag_offset.x;
    float offset_y = mouse.y - drag_state->drag_offset.y;

    ui_element->sprite.x += offset_x;
    ui_element->sprite.y += offset_y;
    ui_element->sprite.z = 10.0f;

    // empty hitbox so that no more hit detection on this
    ui_element->hitbox.x = -INFINITY;
    ui_element->hitbox.y = -INFINITY;
    ui_element->hitbox.width = 0.0f;
    ui_element->hitbox.height = 0.0f;

    return true;
}

static bool can_card_be_moved_from(Game* game, SelectionLocation location, uint8_t card_index) {
    return game_can_move_from(game, location, card_index);
}

static bool can_card_be_moved_to(
    Game* game,
    UIDragState* drag_state,
    SelectionLocation location,
    uint32_t card_index
) {
    if (!drag_state->dragging) {
        return false;
    }

    Move move = {
        .from = drag_state->card_location,
        .to = location,
        .size = freecell_count_cards_from_index(
            &game->freecell,
            drag_state->card_location,
            drag_state->card_index
        ),
    };

    return game_validate_move(game, move) == MOVE_SUCCESS;
}

UIElement ui_get_new_state(
    World* world,
    UIElement* element,
    bool hovered,
    bool clicked,
    bool disabled
) {
    UIElement new_element = *element;
    Game* game = &world->game;
    UIDragState* drag_state = &world->controller.drag_state;

    bool is_dragged = ui_set_dragged_element_properties(&new_element, world);

    if (element->type == UI_CARD) {
        SelectionLocation location = element->meta.card.selection_location;
        uint32_t card_index = element->meta.card.card_index;

        bool is_top_card = true;
        if (selection_location_is_cascade(location)) {
            Cascade* cascade = &game->freecell.cascade[location - CASCADE_1];
            is_top_card = cascade->size > 0 && card_index == cascade->size - 1;
        }

        bool is_target
            = is_top_card && drag_state->dragging && drag_state->card_location != location;

        bool can_move_to
            = is_top_card && can_card_be_moved_to(game, drag_state, location, card_index);
        bool can_move_from = can_card_be_moved_from(game, location, card_index);

        new_element.meta.card.state = ui_card_state_transition(
            element->meta.card.state,
            hovered,
            clicked,
            can_move_from,
            can_move_to,
            is_dragged,
            is_target
        );

    } else if (element->type == UI_BUTTON) {
        const char* id = aptr(element->meta.button.id);
        // Undo button is disabled if no moves left
        // or if game over or potentially solved
        if (strcmp(id, "undo") == 0
            && (world->game.history.size == 0 || freecell_game_over(&world->game.freecell)
                || freecell_is_trivially_solved(&world->game.freecell))) {
            disabled = true;
        }

        new_element.meta.button.state
            = ui_button_state_transition(element->meta.button.state, hovered, clicked, disabled);
    }

    ui_element_apply_style(&new_element);
    return new_element;
}

void ui_update_element_states(World* world) {
    Controller* controller = &world->controller;
    bool pressed = window_is_mouse_pressed(world->window, RGFW_mouseLeft);

    size_t hit_index = -1;
    ui_get_topmost_hit(&world->ui_elements, controller->mouse, NULL, &hit_index);

    for (size_t i = 0; i < world->ui_elements.size; ++i) {
        vec_get_as(UIElement, element, &world->ui_elements, i);

        bool hovered = hit_index == i;
        bool clicked = hovered && pressed;
        bool disabled = false;

        UIElement updated = ui_get_new_state(world, &element, hovered, clicked, disabled);
        vec_set(&world->ui_elements, i, &updated);
    }
}
