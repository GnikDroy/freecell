#pragma once

#include "callbacks.h"
#include "game.h"
#include "vector.h"
#include "world.h"

const int SINGLE_CARD_WIDTH = 88;
const int SINGLE_CARD_HEIGHT = 119;

const int CARDSHEET_OFFSET_X = 5;
const int CARDSHEET_OFFSET_Y = 5;

typedef struct {
  float uv_top, uv_bottom, uv_left, uv_right;
  float x, y;
  float width, height;
  float r, g, b;
} Sprite;

typedef struct {
  Sprite card[53];
} DeckSprite;

DeckSprite generate_deck_sprites(void) {
  const float SCALE = 1.3f;

  DeckSprite deck_sprite = {0};
  const int ROW = 4;
  const int COL = 13;
  const int TOTAL_WIDTH =
      COL * (SINGLE_CARD_WIDTH + CARDSHEET_OFFSET_X) - CARDSHEET_OFFSET_X;
  const int TOTAL_HEIGHT =
      (ROW + 1) * (SINGLE_CARD_HEIGHT + CARDSHEET_OFFSET_Y) -
      CARDSHEET_OFFSET_Y;

  for (int index = 0; index <= 52; index++) {
    deck_sprite.card[index].r = 1.0;
    deck_sprite.card[index].g = 1.0;
    deck_sprite.card[index].b = 1.0;

    deck_sprite.card[index].width = (float)SINGLE_CARD_WIDTH * SCALE;
    deck_sprite.card[index].height = (float)SINGLE_CARD_HEIGHT * SCALE;
  }

  int index = 0;
  deck_sprite.card[index].uv_top =
      1.0f -
      4 * (SINGLE_CARD_HEIGHT + CARDSHEET_OFFSET_Y) / (float)TOTAL_HEIGHT;

  deck_sprite.card[index].uv_bottom =
      deck_sprite.card[index].uv_top -
      (float)SINGLE_CARD_HEIGHT / (float)TOTAL_HEIGHT;

  deck_sprite.card[index].uv_left = 0;
  deck_sprite.card[index].uv_right =
      (float)SINGLE_CARD_WIDTH / (float)TOTAL_WIDTH;

  for (int i = 0; i < COL; i++) {
    for (int j = 0; j < ROW; j++) {
      int index = j * COL + COL - i;
      deck_sprite.card[index].uv_top =
          1.0f -
          j * (SINGLE_CARD_HEIGHT + CARDSHEET_OFFSET_Y) / (float)TOTAL_HEIGHT;

      deck_sprite.card[index].uv_bottom =
          deck_sprite.card[index].uv_top -
          (float)SINGLE_CARD_HEIGHT / (float)TOTAL_HEIGHT;

      deck_sprite.card[index].uv_left =
          i * (SINGLE_CARD_WIDTH + CARDSHEET_OFFSET_X) / (float)TOTAL_WIDTH;

      deck_sprite.card[index].uv_right =
          deck_sprite.card[index].uv_left +
          (float)SINGLE_CARD_WIDTH / (float)TOTAL_WIDTH;
    }
  }

  return deck_sprite;
}

typedef struct {
  GLFWwindow *window;
  uint32_t shader;
  uint32_t VBO;
  uint32_t VAO;
  uint32_t EBO;
  uint32_t textures;
  DeckSprite deck_sprites;
} Renderer;

const char *vertexShaderSource =
    "#version 330 core \n"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec3 aColor;"
    "layout (location = 2) in vec2 tex_coords_in;"
    "out vec3 outColor;"
    "out vec2 tex_coords;"
    "uniform mat4 projection;"
    "void main() {"
    "   gl_Position = projection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "   tex_coords = tex_coords_in;"
    "   outColor = aColor;"
    "}";
const char *fragmentShaderSource =
    "#version 330 core \n"
    "out vec4 FragColor;"
    "in vec3 outColor;"
    "in vec2 tex_coords;"
    "uniform sampler2D spritesheet;"
    "void main()"
    "{"
    "    vec4 texColor = texture(spritesheet, tex_coords);"
    "    FragColor = texColor * vec4(outColor, 1.0);"
    "}";

Renderer renderer_init(GLFWwindow *window, World *world) {
  uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  uint32_t shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  uint32_t VAO;
  glGenVertexArrays(1, &VAO);

  uint32_t VBO;
  glGenBuffers(1, &VBO);

  uint32_t EBO;
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBindVertexArray(0);

  uint32_t textures;
  glGenTextures(1, &textures);
  glBindTexture(GL_TEXTURE_2D, textures);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, world->spritesheet.width,
                 world->spritesheet.height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, world->spritesheet.width,
                  world->spritesheet.height, GL_RGBA, GL_UNSIGNED_BYTE,
                  world->spritesheet.data);

  Renderer renderer = {
      .window = window,
      .shader = shaderProgram,
      .VBO = VBO,
      .VAO = VAO,
      .EBO = EBO,
      .textures = textures,
      .deck_sprites = generate_deck_sprites(),
  };
  return renderer;
}

void renderer_free(Renderer *renderer) {
  glDeleteBuffers(1, &renderer->VBO);
  glDeleteBuffers(1, &renderer->EBO);
  glDeleteTextures(1, &renderer->textures);
  glDeleteVertexArrays(1, &renderer->VAO);
  glDeleteProgram(renderer->shader);
}

void renderer_clear(Renderer renderer) {
  (void)renderer;
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

#pragma pack(push, 1)
typedef struct {
  float x, y, z;
  float r, g, b;
  float u, v;
} Vertex;
#pragma pack(pop)

void mat4_ortho(float left, float right, float bottom, float top, float near,
                float far, float *out) {
  memset(out, 0, sizeof(float) * 16);
  out[0] = 2.0f / (right - left);
  out[5] = 2.0f / (top - bottom);
  out[10] = -2.0f / (far - near);
  out[12] = -(right + left) / (right - left);
  out[13] = -(top + bottom) / (top - bottom);
  out[14] = -(far + near) / (far - near);
  out[15] = 1.0f;
}

void renderer_render_sprite(Sprite sprite, Vector *vertices_vec,
                            Vector *indices_vec) {
  Vertex vertices[4] = {0};
  for (int i = 0; i < 4; i++) {
    vertices[i].r = vertices[i].g = vertices[i].b = 1.0f;
  }

  float x = sprite.x;
  float y = sprite.y;

  float halfW = (float)sprite.width / 2.f;
  float halfH = (float)sprite.height / 2.f;

  vertices[0].x = x + halfW;
  vertices[0].y = y - halfH;

  vertices[1].x = x + halfW;
  vertices[1].y = y + halfH;

  vertices[2].x = x - halfW;
  vertices[2].y = y + halfH;

  vertices[3].x = x - halfW;
  vertices[3].y = y - halfH;

  for (int i = 0; i < 4; i++) {
    vertices[i].z = 0.0f;
  }

  for (int i = 0; i < 4; i++) {
    vertices[i].r = sprite.r;
    vertices[i].g = sprite.g;
    vertices[i].b = sprite.b;
  }

  vertices[0].u = sprite.uv_right;
  vertices[0].v = sprite.uv_top;

  vertices[1].u = sprite.uv_right;
  vertices[1].v = sprite.uv_bottom;

  vertices[2].u = sprite.uv_left;
  vertices[2].v = sprite.uv_bottom;

  vertices[3].u = sprite.uv_left;
  vertices[3].v = sprite.uv_top;

  uint32_t base_index = (uint32_t)vertices_vec->size;
  uint32_t indices[6] = {
      base_index + 0, base_index + 1, base_index + 3,
      base_index + 1, base_index + 2, base_index + 3,
  };

  for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
    vec_push_back(vertices_vec, &vertices[i]);
  }

  for (size_t i = 0; i < 6; i++) {
    vec_push_back(indices_vec, &indices[i]);
  }
}

void renderer_draw_cards(Renderer renderer, Game *game) {
  Vector vertices = vec_init(sizeof(Vertex));
  Vector indices = vec_init(sizeof(uint32_t));

  int window_width, window_height;
  glfwGetWindowSize(renderer.window, &window_width, &window_height);

  // render free cells
  for (int i = 0; i < 4; i++) {
    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 15;
    Card card = game->freecell.reserve[i];
    Sprite sprite = renderer.deck_sprites.card[card];

    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    renderer_render_sprite(sprite, &vertices, &indices);
  }

  // render foundation
  for (int i = 0; i < 4; i++) {
    const int MARGIN_Y = 30;
    const int MARGIN_X = 30;
    const int GAP = 15;
    Card card = game->freecell.foundation[i];
    Sprite sprite = renderer.deck_sprites.card[card];
    if (card == NONE) {
      sprite = renderer.deck_sprites.card[ACE_SPADES + 13 * i];
      sprite.r = 0.5f;
      sprite.g = 0.5f;
      sprite.b = 0.5f;
    }
    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.x = window_width - sprite.x;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    renderer_render_sprite(sprite, &vertices, &indices);
  }

  // render cascades centered
  Sprite sample = renderer.deck_sprites.card[NONE];
  const int cascade_count = 8;
  const int GAP = 15;
  const float total_width =
      cascade_count * sample.width + (cascade_count - 1) * GAP;
  float start_x = (window_width - total_width) / 2.0f;

  for (int i = 0; i < cascade_count; i++) {
    const int MARGIN_Y = SINGLE_CARD_HEIGHT * 2;
    const int MARGIN_X = 0;
    const int OVERLAP = 25;

    Cascade *cascade = &game->freecell.cascade[i];
    for (int j = 0; j < cascade->size; j++) {
      Card card = cascade->cards[j];

      Sprite sprite = renderer.deck_sprites.card[card];
      sprite.x =
          start_x + i * (sprite.width + GAP) + sprite.width / 2.f + MARGIN_X;
      sprite.y = sprite.height / 2.f + MARGIN_Y + OVERLAP * j;

      renderer_render_sprite(sprite, &vertices, &indices);
    }
  }

  // Set the buffer data
  glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size * vertices.elem_size,
               vertices.data, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * indices.elem_size,
               indices.data, GL_STATIC_DRAW);

  // Set the VAO and draw
  glBindVertexArray(renderer.VAO);
  glDrawElements(GL_TRIANGLES, (GLsizei)indices.size, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  vec_free(&vertices);
  vec_free(&indices);
}

void renderer_draw(Renderer renderer, World *world) {
  (void)world;
  glUseProgram(renderer.shader);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer.textures);
  glUniform1i(glGetUniformLocation(renderer.shader, "spritesheet"), 0);
  glUniformMatrix4fv(glGetUniformLocation(renderer.shader, "projection"), 1,
                     GL_FALSE, world->projection);

  renderer_draw_cards(renderer, &world->game);
}
