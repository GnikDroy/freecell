#pragma once
#include <math.h>
#include <stdint.h>
#include "input_action.h"

typedef struct RGFW_window RGFW_window;
typedef struct World World;
typedef struct InputAction InputAction;

bool input_get_input_action(RGFW_window* window, World* world, InputAction* ia);
