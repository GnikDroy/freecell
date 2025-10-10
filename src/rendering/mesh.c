#define _USE_MATH_DEFINES
#include <math.h>
#include <stddef.h>

#include "rendering/mesh.h"
#include <glad/glad.h>

Mesh mesh_init() {
    Mesh mesh;
    mesh.vertices = vec_init(sizeof(Vertex));
    mesh.indices = vec_init(sizeof(uint32_t));
    return mesh;
}

void mesh_clear(Mesh* mesh) {
    mesh->vertices.size = 0;
    mesh->indices.size = 0;
}

void mesh_free(Mesh* mesh) {
    vec_free(&mesh->vertices);
    vec_free(&mesh->indices);
}

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
        vertices[i].z = quad.z;

        vertices[i].r = quad.color.r;
        vertices[i].g = quad.color.g;
        vertices[i].b = quad.color.b;
        vertices[i].a = quad.color.a;
        vertices[i].u = INFINITY;
        vertices[i].v = INFINITY;
    }

    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vertices[i].r = quad.color.r;
        vertices[i].g = quad.color.g;
        vertices[i].b = quad.color.b;
        vertices[i].a = quad.color.a;
    }

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

GPUMesh gpu_mesh_init() {
    GPUMesh mesh;

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glBindVertexArray(mesh.VAO);

    size_t stride = sizeof(Vertex);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);

    // Position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    // Color (location = 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, r));
    glEnableVertexAttribArray(1);

    // TexCoords (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    mesh.index_count = 0;

    return mesh;
}

void gpu_mesh_free(GPUMesh* mesh) {
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->EBO);
    glDeleteVertexArrays(1, &mesh->VAO);
    mesh->VAO = mesh->VBO = mesh->EBO = 0;
    mesh->index_count = 0;
}

void gpu_mesh_upload(GPUMesh* gpu_mesh, Mesh* mesh) {
    glBindVertexArray(gpu_mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->VBO);
    // glBufferData with NULL to orphan the buffer (for performance)
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh->vertices.size * mesh->vertices.elem_size,
        NULL,
        GL_DYNAMIC_DRAW
    );
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh->vertices.size * mesh->vertices.elem_size,
        mesh->vertices.data,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->EBO);
    // glBufferData with NULL to orphan the buffer (for performance)
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh->indices.size * mesh->indices.elem_size,
        NULL,
        GL_DYNAMIC_DRAW
    );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh->indices.size * mesh->indices.elem_size,
        mesh->indices.data,
        GL_DYNAMIC_DRAW
    );
    glBindVertexArray(0);
    gpu_mesh->index_count = mesh->indices.size;
}
