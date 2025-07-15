#include "game/controller.h"

#include "core/game.h"

#include "game/input_action.h"
#include "game/ui_element.h"
#include "platform/window.h"

#include "game/constants.h"
#include "game/render_system.h"
#include "game/ui_layout.h"
#include "game/world.h"

#include "utils.h"

static UIElement controller_get_ui_state(
    World* world, UIElement* element, bool hovered, bool clicked, bool disabled) {

    UIElement new_element = *element;
    Game* game = &world->game;

    if (element->type == UI_CARD) {
        SelectionLocation location = element->meta.card.selection_location;
        uint32_t card_index = element->meta.card.card_index;

        bool can_move = game_can_move_from(game, location, card_index);

        if (clicked && can_move) {
            new_element.meta.card.state = CARD_UI_STATE_SELECTED;
        } else if (hovered && can_move) {
            new_element.meta.card.state = CARD_UI_STATE_HOVERED;
        } else {
            new_element.meta.card.state = CARD_UI_STATE_NORMAL;
        }
    } else if (element->type == UI_BUTTON) {
        if (disabled) {
            new_element.meta.button.state = BUTTON_UI_STATE_DISABLED;
        } else if (clicked) {
            new_element.meta.button.state = BUTTON_UI_STATE_SELECTED;
        } else if (hovered) {
            new_element.meta.button.state = BUTTON_UI_STATE_HOVERED;
        } else {
            new_element.meta.button.state = BUTTON_UI_STATE_NORMAL;
        }
    }

    return new_element;
}

static void controller_update_ui_state(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    bool pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    size_t hit_index = -1;
    ui_get_topmost_hit(&world->ui_elements, controller->mouse, NULL, &hit_index);

    for (size_t i = 0; i < world->ui_elements.size; ++i) {
        vec_get_as(UIElement, element, &world->ui_elements, i);

        bool hovered = hit_index == i;
        bool clicked = hovered && pressed;
        bool disabled = false;
        UIElement expected_state
            = controller_get_ui_state(world, &element, hovered, clicked, disabled);

        if (memcmp(&element, &expected_state, sizeof(UIElement)) != 0) {
            vec_set(&world->ui_elements, i, &expected_state);
            controller->bake_pending = true;
        }
    }
}

static void controller_play_card_move_sound(World* world) {
    ma_sound_stop(&world->card_move_sound);
    ma_sound_start(&world->card_move_sound);
}

static bool is_card_draggable(World* world, UIElement* elem) {
    if (elem->type != UI_CARD)
        return false;

    SelectionLocation loc = elem->meta.card.selection_location;
    uint32_t index = elem->meta.card.card_index;

    return game_can_move_from(&world->game, loc, index);
}

void controller_update(GLFWwindow* window, World* world, double dt) {
    (void)dt;
    Controller* controller = &world->controller;
    controller_update_ui_state(window, world);

    if (controller->layout_pending) {
        layout_world(world);
        bake_world(world);
        controller->layout_pending = false;
        controller->bake_pending = false;
    } else if (controller->bake_pending) {
        bake_world(world);
        controller->bake_pending = false;
    }
}

void controller_start_drag(GLFWwindow* window, World* world) {
    vec2s mouse = world->controller.mouse;

    UIElement ui_element;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &ui_element, NULL)) {
        if (ui_element.type != UI_CARD || !is_card_draggable(world, &ui_element)) {
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

void controller_end_drag(GLFWwindow*, World* world) {
    UIDragState* drag_state = &world->controller.drag_state;

    // handle ui element drop
    Vector* ui_elements = &world->ui_elements;
    UIElement dest;
    if (drag_state->dragging
        && ui_get_topmost_hit(ui_elements, world->controller.mouse, &dest, NULL)
        && dest.type == UI_CARD) {
        controller_handle_card_drop(
            &dest, drag_state->card_location, drag_state->card_index, world);
    }

    // Update drag state and queue layout update
    drag_state->dragging = false;
    drag_state->drag_offset.x = 0;
    drag_state->drag_offset.y = 0;
    drag_state->card_location = -1;
    drag_state->card_index = 0;
    world->controller.layout_pending = true;
}

bool controller_handle_card_drop(
    UIElement* dest, SelectionLocation from_location, uint8_t card_index, World* world) {

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

    MoveResult result = game_move(&world->game,
        (Move) {
            .from = from_location,
            .to = dest_location,
            .size = size,
        });

    if (result == MOVE_SUCCESS) {
        controller_play_card_move_sound(world);
    }

    return result == MOVE_SUCCESS;
}

void controller_smart_move(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;
    vec2s mouse = controller->mouse;

    UIElement topmost;
    size_t index;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &topmost, &index)) {
        if (topmost.type == UI_CARD) {
            Card card = topmost.meta.card.card;
            SelectionLocation location = topmost.meta.card.selection_location;

            if (card == NONE) {
                return;
            }

            uint8_t size = freecell_count_cards_from_index(
                &world->game.freecell, location, topmost.meta.card.card_index);

            // foundation destination is automatically corrected to the suit of the card
            SelectionLocation foundation_destination = get_suit(card) + FOUNDATION_SPADES;

            // if can move to foundation, do it
            if (freecell_validate_to_foundation(&world->game.freecell, card, foundation_destination)
                == MOVE_SUCCESS) {
                game_move(&world->game,
                    (Move) {
                        .from = location,
                        .to = FOUNDATION_SPADES + get_suit(card),
                        .size = size,
                    });
                controller->layout_pending = true;
                return;
            }

            // if can move to any reserve, do it
            for (uint8_t i = 0; i < 4; i++) {
                if (freecell_validate_to_reserve(&world->game.freecell, card, RESERVE_1 + i)
                    == MOVE_SUCCESS) {
                    game_move(&world->game,
                        (Move) {
                            .from = location,
                            .to = RESERVE_1 + i,
                            .size = size,
                        });
                    controller->layout_pending = true;
                    return;
                }
            }
        }
    }
}

void controller_on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    (void)window;
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

void controller_on_cursor_position(GLFWwindow* window, double x, double y) {
    World* world = glfwGetWindowUserPointer(window);
    Controller* controller = &world->controller;

    controller->mouse_screen.x = (float)x;
    controller->mouse_screen.y = (float)y;

    int width, height;
    window_get_size(window, &width, &height);
    controller->mouse = screen_to_world(x, y, width, height, &world->camera);

    if (controller->drag_state.dragging) {
        controller->layout_pending = true;
    }
}

void controller_undo(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    if (game_undo(&world->game) == MOVE_SUCCESS) {
        controller_play_card_move_sound(world);
        controller->layout_pending = true;
    }
}

void controller_new_game(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    game_new(&world->game);
    controller->layout_pending = true;
}

void controller_toggle_fullscreen(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    window_toggle_fullscreen(window);
    controller->layout_pending = true;
}

void controller_toggle_debug(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    controller->debug = !controller->debug;
    controller->layout_pending = true;
}

void controller_complete_game(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    // complete the game for debug
    Freecell* freecell = &world->game.freecell;

    for (int i = 0; i < 8; i++) {
        freecell->cascade[i].size = 0;
    }
    for (int i = 0; i < 4; i++) {
        freecell->reserve[i] = NONE;
    }
    freecell->foundation[SPADES] = KING_SPADES;
    freecell->foundation[HEARTS] = KING_HEARTS;
    freecell->foundation[DIAMONDS] = KING_DIAMONDS;
    freecell->foundation[CLUBS] = KING_CLUBS;
    world->game.history.size = 0;
    controller->layout_pending = true;
}

void controller_fill_cascades(GLFWwindow* window, World* world) {
    Controller* controller = &world->controller;

    // make all cascades full for debugging view
    Freecell* freecell = &world->game.freecell;

    const int MAX_CASCADE_SIZE = sizeof(freecell->cascade[0].cards) / sizeof(Card);
    for (int j = 0; j < 8; j++) {
        freecell->cascade[j].size = 0;
        for (int i = 0; i < MAX_CASCADE_SIZE; i++) {
            cascade_push(&freecell->cascade[j], KING_CLUBS - i);
        }
    }

    world->game.history.size = 0;
    controller->layout_pending = true;
}

void controller_handle_input(InputAction ia) {
    GLFWwindow* window = ia.window;
    World* world = glfwGetWindowUserPointer(window);

    if (ia.type == INPUT_ACTION_FRAMEBUFFER_RESIZE) {
        controller_on_framebuffer_resize(
            window, ia.data.framebuffer_resize.width, ia.data.framebuffer_resize.height);
    } else if (ia.type == INPUT_ACTION_POINTER_MOVE) {
        controller_on_cursor_position(window, ia.data.pointer_move.x, ia.data.pointer_move.y);
    } else if (ia.type == INPUT_ACTION_START_DRAG) {
        controller_start_drag(window, world);
    } else if (ia.type == INPUT_ACTION_END_DRAG) {
        controller_end_drag(window, world);
    } else if (ia.type == INPUT_ACTION_SMART_MOVE) {
        controller_smart_move(window, world);
    } else if (ia.type == INPUT_ACTION_UNDO) {
        controller_undo(window, world);
    } else if (ia.type == INPUT_ACTION_NEW_GAME) {
        controller_new_game(window, world);
    } else if (ia.type == INPUT_ACTION_TOGGLE_FULLSCREEN) {
        controller_toggle_fullscreen(window, world);
    } else if (ia.type == INPUT_ACTION_TOGGLE_DEBUG) {
        controller_toggle_debug(window, world);
    } else if (ia.type == INPUT_ACTION_COMPLETE_GAME) {
        controller_complete_game(window, world);
    } else if (ia.type == INPUT_ACTION_FILL_CASCADES) {
        controller_fill_cascades(window, world);
    }
}
