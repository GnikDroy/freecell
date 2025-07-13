#include "renderer.h"
#include "debug.h"
#include "mesh.h"

Renderer renderer_init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(openglDebugCallback, NULL);

  // Cull back faces
  glEnable(GL_CULL_FACE);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Renderer renderer = {0};
  return renderer;
}

void renderer_free(Renderer *renderer) { (void)renderer; }

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

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive) {
  glBindVertexArray(mesh->VAO);
  glDrawElements(primitive, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
