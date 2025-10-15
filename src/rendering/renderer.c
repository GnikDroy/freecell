#include "rendering/renderer.h"
#include "rendering/mesh.h"

void renderer_init() {
#ifdef FREECELL_DEBUG
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(openglDebugCallback, NULL);
#endif

    // Cull back faces
    glEnable(GL_CULL_FACE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void renderer_clear(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void renderer_set_shader(uint32_t shader) { glUseProgram(shader); }

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, texture);
}

void renderer_draw_mesh(GPUMesh* mesh, GLenum primitive) {
    glBindVertexArray(mesh->VAO);
    glDrawElements(primitive, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#ifdef FREECELL_DEBUG
#include "core/log.h"
void APIENTRY openglDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    (void)length;
    (void)userParam;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    log_error(
        "OpenGL Debug Message:\n"
        "Source: 0x%x\n"
        "Type: 0x%x\n"
        "ID: %u\n"
        "Severity: 0x%x\n"
        "Message: %s\n\n",
        source,
        type,
        id,
        severity,
        message
    );
}
#else
void APIENTRY openglDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
    (void)length;
    (void)message;
    (void)userParam;
}
#endif
