#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "constants.h"
#include "controller.h"
#include "game.h"
#include "render_system.h"
#include "ui_layout.h"
#include "utils.h"
#include "window.h"
#include "world.h"

UIElement compute_state(
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

    size_t hit_index = -1;
    ui_get_topmost_hit(&world->ui_elements, controller->mouse, NULL, &hit_index);

    for (size_t i = 0; i < world->ui_elements.size; ++i) {
        vec_get_as(UIElement, element, &world->ui_elements, i);

        bool hovered = hit_index == i;
        bool pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool clicked = hovered && pressed;
        bool disabled = false;
        UIElement expected_state = compute_state(world, &element, hovered, clicked, disabled);

        if (memcmp(&element, &expected_state, sizeof(UIElement)) != 0) {
            vec_set(&world->ui_elements, i, &expected_state);
            controller->bake_pending = true;
        }
    }
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

void controller_start_drag(World* world) {
    Vector* ui_elements = &world->ui_elements;
    UIDragState* drag_state = &world->controller.drag_state;
    vec2s mouse = world->controller.mouse;

    UIElement topmost;
    size_t index;

    if (ui_get_topmost_hit(ui_elements, mouse, &topmost, &index)) {
        // Only start dragging if the topmost element is a card
        if (topmost.type != UI_CARD) {
            return;
        }

        SelectionLocation location = topmost.meta.card.selection_location;
        uint32_t card_index = topmost.meta.card.card_index;

        // Only start dragging if the movement is likely allowed
        if (!game_can_move_from(&world->game, location, card_index)) {
            return;
        }

        drag_state->dragging = true;
        drag_state->ui_elements_index = index;

        drag_state->drag_offset.x = mouse.x - topmost.sprite.x;
        drag_state->drag_offset.y = mouse.y - topmost.sprite.y;

        drag_state->original_position.x = topmost.sprite.x;
        drag_state->original_position.y = topmost.sprite.y;
    }
}

void controller_update_drag(World* world) {
    UIDragState* drag_state = &world->controller.drag_state;
    vec2s mouse = world->controller.mouse;

    if (!drag_state->dragging)
        return;

    Vector* ui_elements = &world->ui_elements;

    vec_get_as(UIElement, elem, ui_elements, drag_state->ui_elements_index);

    elem.sprite.x = mouse.x - drag_state->drag_offset.x;
    elem.sprite.y = mouse.y - drag_state->drag_offset.y;
    elem.sprite.z = 10.0f;

    // empty hitbox so that no more hit detection on this
    elem.hitbox.x = -INFINITY;
    elem.hitbox.y = -INFINITY;
    elem.hitbox.width = 0.0f;
    elem.hitbox.height = 0.0f;

    vec_set(ui_elements, drag_state->ui_elements_index, &elem);
}

void controller_end_drag(World* world) {
    UIDragState* drag_state = &world->controller.drag_state;

    if (!drag_state->dragging)
        return;

    drag_state->dragging = false;

    // Queue bake for the world
    world->controller.bake_pending = true;

    // Get dragged ui element
    Vector* ui_elements = &world->ui_elements;
    vec_get_as(UIElement, from, ui_elements, drag_state->ui_elements_index);

    // Attempt to move to topmost element
    vec2s mouse = world->controller.mouse;

    UIElement dest;
    if (ui_get_topmost_hit(ui_elements, mouse, &dest, NULL)) {
        if (dest.type == UI_CARD) {
            SelectionLocation from_location = from.meta.card.selection_location;
            SelectionLocation dest_location = dest.meta.card.selection_location;

            uint8_t size = 1;

            if (selection_location_is_cascade(from_location)) {
                size = world->game.freecell.cascade[from_location - CASCADE_1].size
                    - from.meta.card.card_index;
            }

            Move move = {
                .from = from_location,
                .to = dest_location,
                .size = size,
            };

            MoveResult result = game_move(&world->game, move);

            if (result == MOVE_SUCCESS) {
                world->controller.layout_pending = true;
                return;
            }
        }
    }

    // Reset original position and hitbox
    from.sprite.x = drag_state->original_position.x;
    from.sprite.y = drag_state->original_position.y;
    from.sprite.z = 0.0f;
    from.hitbox = compute_hitbox(&from.sprite);
    vec_set(ui_elements, drag_state->ui_elements_index, &from);
}

void controller_on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    World* world = glfwGetWindowUserPointer(window);
    Controller* controller = &world->controller;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            game_undo(&world->game);
            controller->layout_pending = true;
        } else if (key == GLFW_KEY_N) {
            game_new(&world->game);
            controller->layout_pending = true;
        } else if (key == GLFW_KEY_M) {
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
        } else if (key == GLFW_KEY_F) {
            window_toggle_fullscreen(window);
        } else if (key == GLFW_KEY_D) {
            controller->debug = !controller->debug;
        }
    }
}

void controller_smart_move(World* world) {
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

            uint8_t size = 1;
            if (selection_location_is_cascade(location)) {
                size = world->game.freecell.cascade[location - CASCADE_1].size
                    - topmost.meta.card.card_index;
            }

            if (freecell_validate_to_foundation(&world->game.freecell, card) == MOVE_SUCCESS) {
                Move move = {
                    .from = location,
                    .to = FOUNDATION_SPADES + get_suit(card),
                    .size = size,
                };
                game_move(&world->game, move);
                controller->layout_pending = true;
                return;
            } else {
                for (uint8_t i = 0; i < 4; i++) {
                    if (freecell_validate_to_reserve(&world->game.freecell, card, RESERVE_1 + i)
                        == MOVE_SUCCESS) {
                        Move move = {
                            .from = location,
                            .to = RESERVE_1 + i,
                            .size = size,
                        };
                        game_move(&world->game, move);
                        controller->layout_pending = true;
                        return;
                    }
                }
            }
        }
    }
}

void controller_on_mouse_click(GLFWwindow* window, int code, int state, int mods) {
    (void)mods;

    World* world = glfwGetWindowUserPointer(window);
    if (code == GLFW_MOUSE_BUTTON_LEFT) {
        if (state == GLFW_PRESS) {
            controller_start_drag(world);
        } else if (state == GLFW_RELEASE) {
            controller_end_drag(world);
        }
    } else if (code == GLFW_MOUSE_BUTTON_RIGHT) {
        if (state == GLFW_PRESS) {
            controller_smart_move(world);
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
        controller_update_drag(world);
        controller->bake_pending = true;
    }
}
