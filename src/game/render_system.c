#include <math.h>
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

void mesh_push_line(Mesh* mesh, Line line) {
    if (line.thickness <= 0.0f) {
        return;
    }

    float dx = line.x2 - line.x1;
    float dy = line.y2 - line.y1;
    float length = sqrtf(dx * dx + dy * dy);

    if (length == 0.0f) {
        return;
    }

    float cx = (line.x1 + line.x2) / 2.0f;
    float cy = (line.y1 + line.y2) / 2.0f;

    float angle = atan2f(dy, dx);

    Quad quad = {
        .x = cx,
        .y = cy,
        .z = (line.z1 + line.z2) / 2.0f,
        .width = length,
        .height = line.thickness,
        .rotation = angle,
        .color = line.color,
    };

    mesh_push_quad(mesh, quad);
}

void mesh_push_triangle(Mesh* mesh, Triangle triangle) {
    Vertex vertices[3] = { 0 };
    for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vertices[i].r = triangle.color.r;
        vertices[i].g = triangle.color.g;
        vertices[i].b = triangle.color.b;
        vertices[i].a = triangle.color.a;
        vertices[i].u = INFINITY;
        vertices[i].v = INFINITY;
    }

    vertices[0].x = triangle.x1;
    vertices[0].y = triangle.y1;
    vertices[0].z = triangle.z1;

    vertices[1].x = triangle.x2;
    vertices[1].y = triangle.y2;
    vertices[1].z = triangle.z2;

    vertices[2].x = triangle.x3;
    vertices[2].y = triangle.y3;
    vertices[2].z = triangle.z3;

    uint32_t base_index = (uint32_t)mesh->vertices.size;
    uint32_t indices[] = {
        base_index,
        base_index + 1,
        base_index + 2,
    };

    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vec_push_back(&mesh->vertices, &vertices[i]);
    }

    for (size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); i++) {
        vec_push_back(&mesh->indices, &indices[i]);
    }
}

void mesh_push_quad(Mesh* mesh, Quad quad) {
    Vertex vertices[4] = { 0 };

    float x = quad.x;
    float y = quad.y;

    float halfW = quad.width / 2.f;
    float halfH = quad.height / 2.f;

    float angle = quad.rotation;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    float localX[4] = { +halfW, +halfW, -halfW, -halfW };
    float localY[4] = { -halfH, +halfH, +halfH, -halfH };

    float worldX[4];
    float worldY[4];

    for (int i = 0; i < 4; ++i) {
        worldX[i] = x + localX[i] * cosA - localY[i] * sinA;
        worldY[i] = y + localX[i] * sinA + localY[i] * cosA;
    }

    float z = quad.z;
    Color color = quad.color;

    mesh_push_triangle(
        mesh,
        (Triangle) {
            .x1 = worldX[0],
            .y1 = worldY[0],
            .z1 = z,
            .x2 = worldX[2],
            .y2 = worldY[2],
            .z2 = z,
            .x3 = worldX[1],
            .y3 = worldY[1],
            .z3 = z,
            .color = color,
        }
    );

    mesh_push_triangle(
        mesh,
        (Triangle) {
            .x1 = worldX[0],
            .y1 = worldY[0],
            .z1 = z,
            .x2 = worldX[3],
            .y2 = worldY[3],
            .z2 = z,
            .x3 = worldX[2],
            .y3 = worldY[2],
            .z3 = z,
            .color = color,
        }
    );
}

void mesh_push_ellipse(Mesh* mesh, Ellipse ellipse) {
    if (ellipse.radius_x <= 0.0f || ellipse.radius_y <= 0.0f || ellipse.segments <= 3) {
        return;
    }

    uint32_t base_index = (uint32_t)mesh->vertices.size;

    Vertex center_vertex = { 0 };
    center_vertex.x = ellipse.x;
    center_vertex.y = ellipse.y;
    center_vertex.z = ellipse.z;
    center_vertex.r = ellipse.color.r;
    center_vertex.g = ellipse.color.g;
    center_vertex.b = ellipse.color.b;
    center_vertex.a = ellipse.color.a;
    center_vertex.u = INFINITY;
    center_vertex.v = INFINITY;

    vec_push_back(&mesh->vertices, &center_vertex);

    for (int i = 0; i < ellipse.segments; i++) {
        float theta = ((float)i / (float)ellipse.segments) * 2.0f * M_PI;
        float x = ellipse.radius_x * cosf(theta);
        float y = ellipse.radius_y * sinf(theta);

        Vertex vertex = center_vertex;
        vertex.x = ellipse.x + x * cosf(ellipse.rotation) - y * sinf(ellipse.rotation);
        vertex.y = ellipse.y + x * sinf(ellipse.rotation) + y * cosf(ellipse.rotation);

        vec_push_back(&mesh->vertices, &vertex);
    }

    for (int i = 0; i < ellipse.segments; i++) {
        uint32_t i0 = base_index;
        uint32_t i1 = base_index + 1 + i;
        uint32_t i2 = base_index + 1 + ((i + 1) % ellipse.segments);

        vec_push_back(&mesh->indices, &i0);
        vec_push_back(&mesh->indices, &i2);
        vec_push_back(&mesh->indices, &i1);
    }
}

void mesh_push_circle(Mesh* mesh, Circle circle) {
    mesh_push_ellipse(
        mesh,
        (Ellipse) {
            .x = circle.x,
            .y = circle.y,
            .z = circle.z,
            .radius_x = circle.radius,
            .radius_y = circle.radius,
            .segments = circle.segments,
            .rotation = 0,
            .color = circle.color,
        }
    );
}

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
