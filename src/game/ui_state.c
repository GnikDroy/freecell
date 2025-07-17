#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "game/ui_state.h"

#include "core/game.h"
#include "game/ui_element.h"
#include "game/ui_layout.h"
#include "game/world.h"
#include "platform/window.h"

static void ui_element_apply_style(UIElement* ui_element) {
    if (ui_element->type == UI_CARD) {
        if (ui_element->meta.card.state == CARD_UI_STATE_SELECTED) {
            ui_element->sprite.color.r = 0.6f;
            ui_element->sprite.color.g = 0.6f;
            ui_element->sprite.color.b = 0.6f;
        } else if (ui_element->meta.card.state == CARD_UI_STATE_HOVERED) {
            ui_element->sprite.color.r = 0.8f;
            ui_element->sprite.color.g = 0.8f;
            ui_element->sprite.color.b = 0.8f;
        }
    }
}

static CardUIState ui_card_state_transition(
    CardUIState current,
    bool hovered,
    bool clicked,
    bool can_move
) {
    if (clicked && can_move) {
        return CARD_UI_STATE_SELECTED;
    } else if (hovered && can_move) {
        return CARD_UI_STATE_HOVERED;
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

bool ui_set_element_drag_properties(UIElement* ui_element, World* world) {
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

UIElement ui_get_new_state(
    World* world,
    UIElement* element,
    bool hovered,
    bool clicked,
    bool disabled
) {
    UIElement new_element = *element;
    Game* game = &world->game;

    if (ui_set_element_drag_properties(&new_element, world)) {
        hovered = false;
        clicked = true;
    }

    if (element->type == UI_CARD) {
        SelectionLocation location = element->meta.card.selection_location;
        uint32_t card_index = element->meta.card.card_index;

        bool can_move = game_can_move_from(game, location, card_index);

        if (world->controller.drag_state.dragging) {
            UIDragState* drag_state = &world->controller.drag_state;
            SelectionLocation from_location = drag_state->card_location;
            uint32_t from_index = drag_state->card_index;

            if (from_location != location) {
                can_move = game_validate_move(
                               game,
                               (Move) {
                                   .from = from_location,
                                   .to = location,
                                   .size = freecell_count_cards_from_index(
                                       &game->freecell,
                                       from_location,
                                       from_index
                                   ),
                               }
                           )
                    == MOVE_SUCCESS;
            }
        }

        new_element.meta.card.state
            = ui_card_state_transition(element->meta.card.state, hovered, clicked, can_move);
    } else if (element->type == UI_BUTTON) {
        new_element.meta.button.state
            = ui_button_state_transition(element->meta.button.state, hovered, clicked, disabled);
    }

    ui_element_apply_style(&new_element);
    return new_element;
}

void ui_update_element_states(World* world) {
    Controller* controller = &world->controller;
    bool pressed
        = window_get_mouse_button_state(world->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

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
