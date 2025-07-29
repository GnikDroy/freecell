#include "game/debug.h"
#include "game/ui_layout.h"
#include "game/world.h"

#ifdef FREECELL_DEBUG
#include <string.h>

#include "game/ui_element.h"
#include "rendering/renderer.h"

void debug_render_mouse(World* world) {
    Mesh mesh = mesh_init();
    GPUMesh gpu_mesh = gpu_mesh_init();

    Vertex vertex = {
        .x = world->controller.mouse.x,
        .y = world->controller.mouse.y,
        .z = 0,
        .r = 1.0f,
        .g = 0.0f,
        .b = 0.0f,
        .a = 1.0f,
        .u = 0.2f,
        .v = 0.0f,
    };

    vec_push_back(&mesh.vertices, &vertex);

    uint32_t index = 0;
    vec_push_back(&mesh.indices, &index);

    upload_mesh(&gpu_mesh, &mesh);

    glPointSize(10.0f);
    renderer_draw_mesh(&gpu_mesh, GL_POINTS);

    gpu_mesh_free(&gpu_mesh);
    mesh_free(&mesh);
}

static void mesh_push_hitbox(Mesh* mesh, Rect hitbox, Color color) {
    Vertex vertices[4] = { 0 };

    float x = hitbox.x;
    float y = hitbox.y;

    float halfW = hitbox.width / 2.f;
    float halfH = hitbox.height / 2.f;

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
        vertices[i].r = color.r;
        vertices[i].g = color.g;
        vertices[i].b = color.b;
        vertices[i].a = color.a;
        vertices[i].u = 0.2f;
        vertices[i].v = 0.0f;
    }

    uint32_t base_index = (uint32_t)mesh->vertices.size;
    uint32_t indices[8] = {
        base_index + 0, base_index + 1, base_index + 1, base_index + 2,
        base_index + 2, base_index + 3, base_index + 3, base_index + 0,
    };

    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vec_push_back(&mesh->vertices, &vertices[i]);
    }

    for (size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); i++) {
        vec_push_back(&mesh->indices, &indices[i]);
    }
}

void debug_render_hit_hitbox(World* world) {
    Mesh mesh = mesh_init();
    GPUMesh gpu_mesh = gpu_mesh_init();

    vec2s mouse = world->controller.mouse;

    Color color = (Color) { 100.0f, 0.0f, 0.0f, 10.0f };

    UIElement topmost_ui_element;
    if (ui_get_topmost_hit(&world->ui_elements, mouse, &topmost_ui_element, NULL)) {
        mesh_push_hitbox(&mesh, topmost_ui_element.hitbox, color);
    }

    upload_mesh(&gpu_mesh, &mesh);
    renderer_draw_mesh(&gpu_mesh, GL_LINES);

    gpu_mesh_free(&gpu_mesh);
    mesh_free(&mesh);
}
#else
void debug_render_mouse(World* world) { (void)world; }

void debug_render_hit_hitbox(World* world) { (void)world; }
#endif
