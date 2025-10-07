#include <stddef.h>

#include <glad/glad.h>
#include "rendering/mesh.h"

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

void upload_mesh(GPUMesh* gpu_mesh, Mesh* mesh) {
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
