#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 tex_coords_in;

out vec4 outColor;
out vec2 tex_coords;

uniform mat4 projection;

void main() {
   gl_Position = projection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   tex_coords = tex_coords_in;
   outColor = aColor;
}
