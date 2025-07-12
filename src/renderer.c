#include "renderer.h"
#include "debug.h"
#include "mesh.h"

Renderer renderer_init(GLFWwindow *window) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(openglDebugCallback, NULL);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Renderer renderer = {
      .window = window,
  };

  return renderer;
}

void renderer_free(Renderer *renderer) { (void)renderer; }

void renderer_clear() {
  glClearColor(0.2f, 0.4f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_set_shader(uint32_t shader) { glUseProgram(shader); }

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(target, texture);
}

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive) {
  glBindVertexArray(mesh->VAO);
  glDrawElements(primitive, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
