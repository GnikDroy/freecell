#pragma once

#include "game/ui_element.h"

typedef struct World World;
typedef struct GLFWwindow GLFWwindow;

bool ui_is_element_draggable(World* world, UIElement* elem);

bool ui_set_element_drag_properties(UIElement* ui_element, World* world);

UIElement ui_get_new_state(
    World* world, UIElement* element, bool hovered, bool clicked, bool disabled);

void ui_update_element_states(World* world);
