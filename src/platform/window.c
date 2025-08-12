#include <stdlib.h>

#include "platform/window.h"

#include "GLFW/glfw3.h"
#include "core/log.h"

#include "game/constants.h"

static int glfw_initialized = GLFW_FALSE;

static void glfwErrCallback(int error_code, const char* msg) {
    log_error("GLFW Error %d : %s", error_code, msg);
}

static int glfw_initialize() {
    glfwSetErrorCallback(glfwErrCallback);
    if (glfw_initialized) {
        return GLFW_TRUE;
    }

    int glfw_initialized = glfwInit();
    if (glfw_initialized != GLFW_TRUE) {
        exit(EXIT_FAILURE);
    }
    return glfw_initialized;
}

GLFWmonitor* get_primary_monitor() {
    glfw_initialize();
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    if (primary_monitor == NULL) {
        log_error("Failed to get primary monitor.");
        exit(EXIT_FAILURE);
    }
    return primary_monitor;
}

GLFWwindow* window_init(WindowConfig config) {
    glfw_initialize();
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(
        (int)config.width,
        (int)config.height,
        config.title,
        config.fullscreen_monitor,
        NULL
    );
    if (!window) {
        glfwTerminate();
    }
    glfwSetWindowSizeLimits(
        window,
        config.min_width,
        config.min_height,
        config.max_width,
        config.max_height
    );
    if (config.on_close)
        glfwSetWindowCloseCallback(window, config.on_close);
    if (config.on_window_resize)
        glfwSetWindowSizeCallback(window, config.on_window_resize);
    if (config.on_framebuffer_resize)
        glfwSetFramebufferSizeCallback(window, config.on_framebuffer_resize);
    if (config.on_key)
        glfwSetKeyCallback(window, config.on_key);
    if (config.on_mouse_click)
        glfwSetMouseButtonCallback(window, config.on_mouse_click);
    if (config.on_cursor_position)
        glfwSetCursorPosCallback(window, config.on_cursor_position);

    // Set window position to center of monitor
    GLFWmonitor* monitor
        = config.fullscreen_monitor == NULL ? get_primary_monitor() : config.fullscreen_monitor;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (mode->width - config.width) / 2, (mode->height - config.height) / 2);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.vsync);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_error("Failed to initialize GLAD.\n");
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, (int32_t)config.width, (int32_t)config.height);
    return window;
}

void window_free(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void window_get_size(GLFWwindow* window, int* width, int* height) {
    glfwGetWindowSize(window, width, height);
}

const char* window_get_clipboard(GLFWwindow* window) { return glfwGetClipboardString(window); }

void window_toggle_fullscreen(GLFWwindow* window) {
    GLFWmonitor* monitor = window_get_current_monitor(window);
    if (monitor == NULL) {
        monitor = get_primary_monitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        return;
    } else {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        int screenWidth = mode->width;
        int screenHeight = mode->height;

        int xpos = (screenWidth - VIRTUAL_WIDTH) / 2;
        int ypos = (screenHeight - VIRTUAL_HEIGHT) / 2;
        glfwSetWindowMonitor(window, NULL, xpos, ypos, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, 0);
    }
};

void window_maximize(GLFWwindow* window) { glfwMaximizeWindow(window); }

GLFWmonitor* window_get_current_monitor(GLFWwindow* window) { return glfwGetWindowMonitor(window); }

void window_get_cursor_position(GLFWwindow* window, double* x, double* y) {
    glfwGetCursorPos(window, x, y);
}

int window_get_mouse_button_state(GLFWwindow* window, int button) {
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}

void event_post_empty() { return glfwPostEmptyEvent(); }
