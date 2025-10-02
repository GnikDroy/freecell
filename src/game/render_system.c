#include <stdint.h>
#include <string.h>

#include "game/render_system.h"

#include "core/vector.h"

#include "game/animation.h"
#include "game/ui_state.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"

#include "game/debug.h"
#include "game/ui_element.h"
#include "game/ui_layout.h"
#include "game/world.h"

void mesh_push_sprite(Mesh* mesh, Sprite sprite) {
    Vertex vertices[4] = { 0 };

    float x = sprite.x;
    float y = sprite.y;

    float halfW = sprite.width / 2.f;
    float halfH = sprite.height / 2.f;

    float angle = sprite.rotation;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    // Define the 4 corners relative to center
    float localX[4] = { +halfW, +halfW, -halfW, -halfW };
    float localY[4] = { -halfH, +halfH, +halfH, -halfH };

    for (int i = 0; i < 4; i++) {
        float lx = localX[i];
        float ly = localY[i];

        float rx = lx * cosA - ly * sinA;
        float ry = lx * sinA + ly * cosA;

        vertices[i].x = x + rx;
        vertices[i].y = y + ry;
        vertices[i].z = sprite.z;

        vertices[i].r = sprite.color.r;
        vertices[i].g = sprite.color.g;
        vertices[i].b = sprite.color.b;
        vertices[i].a = sprite.color.a;
    }

    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vertices[i].r = sprite.color.r;
        vertices[i].g = sprite.color.g;
        vertices[i].b = sprite.color.b;
        vertices[i].a = sprite.color.a;
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
    uint32_t indices[] = {
        base_index + 3, base_index + 1, base_index + 0,
        base_index + 3, base_index + 2, base_index + 1,
    };

    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vec_push_back(&mesh->vertices, &vertices[i]);
    }

    for (size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); i++) {
        vec_push_back(&mesh->indices, &indices[i]);
    }
}

void mesh_push_ui_element(Mesh* mesh, World* world, UIElement* ui_element) {
    if (ui_element->type == UI_TEXT) {
        mesh_push_text(mesh, world, ui_element);
    } else {
        mesh_push_sprite(mesh, ui_element->sprite);
    }
}

void mesh_push_ui_elements(Mesh* mesh, World* world, Vector* ui_elements) {
    AnimationSystem* anim_sys = &world->animation_system;
    for (size_t i = 0; i < ui_elements->size; i++) {
        vec_get_as(UIElement, element, ui_elements, i);
        if (!animation_system_is_animated(anim_sys, &element)) {
            mesh_push_ui_element(mesh, world, &element);
        }
    }
}

void mesh_push_animations(Mesh* mesh, World* world) {
    // animate ui elements
    AnimationSystem* animation_system = &world->animation_system;
    for (size_t i = 0; i < animation_system->ui_animations.size; i++) {
        vec_get_as(UIElementAnimation, animation, &animation_system->ui_animations, i);
        UIElement ui_element = animation_system_get_next_frame(animation_system, &animation);
        if (animation.elapsed > 0) {
            mesh_push_ui_element(mesh, world, &ui_element);
        }
    }
}

void mesh_push_text(Mesh* mesh, World* world, UIElement* ui_element) {
    float x = ui_element->sprite.x;
    float y = ui_element->sprite.y;
    const char* text = aptr(ui_element->meta.text.text);

    float offset_x = 0;
    float offset_y = 0;

    // assumes font width and height are the same for all characters
    // this is true for the fonts used in this game
    float font_size = world->characters[' '].height * ui_element->meta.text.font_scaling;

    float char_spacing = font_size / 2.0f * ui_element->meta.text.character_spacing_scaling;
    float line_height = font_size * ui_element->meta.text.line_height_scaling;

    size_t string_len = strlen(text);
    for (size_t i = 0; i < string_len; i++) {
        uint8_t c = text[i];
        if (c == '\n') {
            offset_x = 0;
            offset_y += line_height;
        } else if (c == ' ') {
            offset_x += char_spacing;
        } else if (c == '\t') {
            // one tab should be equal to 4 spaces :P
            offset_x += char_spacing * 4;
        } else {
            Sprite sprite = world->characters[c];
            sprite.color = ui_element->sprite.color;
            sprite.x = offset_x + x;
            sprite.y = offset_y + y;
            sprite.z = ui_element->sprite.z;
            sprite.width = font_size;
            sprite.height = font_size;
            mesh_push_sprite(&world->game_mesh, sprite);
            offset_x += char_spacing;
        }
    }
}

static int set_state = false;
static void render_set_static_state_once(World* world) {
    if (!set_state) {
        set_state = true;
        renderer_bind_texture(0, GL_TEXTURE_2D, world->assets.spritesheet_texture);
        renderer_set_shader(world->assets.main_shader);
        shader_set_mat4(world->assets.main_shader, "view", (const float*)world->camera.view);
        shader_set_mat4(
            world->assets.main_shader,
            "projection",
            (const float*)world->camera.projection
        );
        shader_set_int(world->assets.main_shader, "spritesheet", 0);
    }
}

void render_world(World* world) {
    // layout the world
    world->ui_elements.size = 0;
    ui_push_world(&world->ui_elements, world);

    // update ui elements state
    ui_update_element_states(world);

    // create game mesh and upload to gpu
    mesh_clear(&world->game_mesh);
    mesh_push_ui_elements(&world->game_mesh, world, &world->ui_elements);
    mesh_push_animations(&world->game_mesh, world);
    upload_mesh(&world->game_gpu_mesh, &world->game_mesh);

    // draw
    renderer_clear(BACKGROUND_COLOR);
    render_set_static_state_once(world);
    renderer_draw_mesh(&world->game_gpu_mesh, GL_TRIANGLES);

#ifdef FREECELL_DEBUG
    debug_render_mouse(world);
    debug_render_hit_hitbox(world);
#endif
}
