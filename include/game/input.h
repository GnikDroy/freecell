#pragma once
#include <math.h>
#include <stdint.h>

typedef struct GLFWwindow GLFWwindow;

void input_on_key(GLFWwindow* window, int key, int scancode, int action, int mods) ;

void input_on_mouse_click(GLFWwindow* window, int code, int state, int mods) ;

void input_on_framebuffer_resize(GLFWwindow* window, int width, int height) ;

void input_on_cursor_position(GLFWwindow* window, double x, double y) ;
