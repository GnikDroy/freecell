#include "render_system.h"
#include "constants.h"
#include "renderer.h"
#include "shader.h"

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

static void mesh_create_world(Mesh *mesh, World *world) {

  Sprite *deck = world->assets.deck;
  Freecell *freecell = &world->game.freecell;

  // render free cells
  for (int i = 0; i < 4; i++) {
    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 15;
    Card card = freecell->reserve[i];
    Sprite sprite = deck[card];
    if (card == NONE) {
      sprite.a = 0.5f;
    }
    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    mesh_push_sprite(mesh, sprite);
  }

  // render foundation
  for (int i = 0; i < 4; i++) {
    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 15;
    Card card = freecell->foundation[i];
    Sprite sprite = deck[card];
    if (card == NONE) {
      sprite = deck[ACE_SPADES + 13 * i];
      sprite.a = 0.5f;
    }
    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.x = VIRTUAL_WIDTH - sprite.x;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    mesh_push_sprite(mesh, sprite);
  }

  // render cascades centered
  Sprite sample = deck[NONE];
  const int cascade_count = 8;
  const int GAP = 15;
  const float total_width =
      cascade_count * sample.width + (cascade_count - 1) * GAP;
  float start_x = (VIRTUAL_WIDTH - total_width) / 2.0f;

  for (int i = 0; i < cascade_count; i++) {
    const int MARGIN_Y = sample.height * 2;
    const int MARGIN_X = 0;
    const int OVERLAP = 20;

    Cascade *cascade = &freecell->cascade[i];
    for (int j = 0; j < cascade->size; j++) {
      Card card = cascade->cards[j];

      Sprite sprite = deck[card];

      sprite.x =
          start_x + i * (sprite.width + GAP) + sprite.width / 2.f + MARGIN_X;
      sprite.y = sprite.height / 2.f + MARGIN_Y + OVERLAP * j;

      mesh_push_sprite(mesh, sprite);
    }
  }
}

void renderer_draw(World *world) {
  renderer_bind_texture(0, GL_TEXTURE_2D, world->assets.spritesheet_texture);

  renderer_set_shader(world->assets.main_shader);
  shader_set_mat4(world->assets.main_shader, "view", world->camera.view);
  shader_set_mat4(world->assets.main_shader, "projection",
                  world->camera.projection);
  shader_set_int(world->assets.main_shader, "spritesheet", 0);

  Mesh mesh = mesh_init();

  mesh_create_world(&mesh, world);
  upload_mesh(&world->assets.game_mesh, &mesh);
  renderer_draw_mesh(&world->assets.game_mesh, GL_TRIANGLES);

  mesh_free(&mesh);
}
