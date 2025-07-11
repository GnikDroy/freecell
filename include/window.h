#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


typedef struct WindowConfig_t {
    size_t width;
    size_t height;
    size_t min_width;
    size_t min_height;
    size_t max_width;
    size_t max_height;
    const char *title;
    bool vsync;

    void (*on_close)(GLFWwindow *);

    void (*on_window_resize)(GLFWwindow *, int x, int y);

    void (*on_mouse_click)(GLFWwindow *, int code, int state, int mods);
} WindowConfig;

static inline void glfwErrCallback(int error_code, const char *msg) { printf("GLFW Error %d : %s", error_code, msg); }

static inline GLFWwindow *window_init(WindowConfig config) {
    glfwInit();
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    GLFWwindow *window = glfwCreateWindow((int) config.width, (int) config.height, config.title, NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwSetErrorCallback(glfwErrCallback);
    glfwSetWindowSizeLimits(window, config.min_width, config.min_height, config.max_width, config.max_height);
    if (config.on_close) glfwSetWindowCloseCallback(window, config.on_close);
    if (config.on_window_resize) glfwSetWindowSizeCallback(window, config.on_window_resize);
    if (config.on_mouse_click) glfwSetMouseButtonCallback(window, config.on_mouse_click);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.vsync);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD.\n");
    } else {
        printf("GLAD initialization successful.\n");
    }

    printf("OpenGL version is %s\n", glGetString(GL_VERSION));


    glViewport(0, 0, (int32_t) config.width, (int32_t) config.height);
    glEnable(GL_MULTISAMPLE);
    return window;
}

void window_free(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
