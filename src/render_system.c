#include <stdint.h>
#include <string.h>

#include "render_system.h"

#include "renderer.h"
#include "shader.h"
#include "ui_element.h"
#include "ui_layout.h"

void mesh_push_sprite(Mesh *mesh, Sprite sprite) {
  Vertex vertices[4] = {0};

  float x = sprite.x;
  float y = sprite.y;

  float halfW = sprite.width / 2.f;
  float halfH = sprite.height / 2.f;

  vertices[0].x = x + halfW;
  vertices[0].y = y - halfH;
  vertices[0].z = 0.0f;

  vertices[1].x = x + halfW;
  vertices[1].y = y + halfH;
  vertices[1].z = 0.0f;

  vertices[2].x = x - halfW;
  vertices[2].y = y + halfH;
  vertices[2].z = 0.0f;

  vertices[3].x = x - halfW;
  vertices[3].y = y - halfH;
  vertices[3].z = 0.0f;

  for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
    vertices[i].r = sprite.r;
    vertices[i].g = sprite.g;
    vertices[i].b = sprite.b;
    vertices[i].a = sprite.a;
  }

  vertices[0].u = sprite.uv_right;
  vertices[0].v = sprite.uv_top;

  vertices[1].u = sprite.uv_right;
  vertices[1].v = sprite.uv_bottom;

  vertices[2].u = sprite.uv_left;
  vertices[2].v = sprite.uv_bottom;

  vertices[3].u = sprite.uv_left;
  vertices[3].v = sprite.uv_top;

  uint32_t base_index = (uint32_t)mesh->vertices.size;
  uint32_t indices[6] = {
      base_index + 0, base_index + 1, base_index + 3,
      base_index + 1, base_index + 2, base_index + 3,
  };

  for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
    vec_push_back(&mesh->vertices, &vertices[i]);
  }

  for (size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); i++) {
    vec_push_back(&mesh->indices, &indices[i]);
  }
}

void mesh_push_ui_elements(Mesh *mesh, Vector *vec) {
  for (size_t i = 0; i < vec->size; i++) {
    UIElement ui_element;
    memcpy(&ui_element, (uint8_t *)vec->data + i * vec->elem_size,
           vec->elem_size);
    mesh_push_sprite(mesh, ui_element.sprite);
  }
}

void renderer_draw(World *world) {
  renderer_bind_texture(0, GL_TEXTURE_2D, world->assets.spritesheet_texture);

  renderer_set_shader(world->assets.main_shader);
  shader_set_mat4(world->assets.main_shader, "view", world->camera.view);
  shader_set_mat4(world->assets.main_shader, "projection",
                  world->camera.projection);
  shader_set_int(world->assets.main_shader, "spritesheet", 0);

  Vector ui_elements = vec_init(sizeof(UIElement));
  ui_push_world(&ui_elements, world);

  Mesh mesh = mesh_init();
  mesh_push_ui_elements(&mesh, &ui_elements);

  upload_mesh(&world->assets.game_mesh, &mesh);
  renderer_draw_mesh(&world->assets.game_mesh, GL_TRIANGLES);

  vec_free(&ui_elements);
  mesh_free(&mesh);
}
