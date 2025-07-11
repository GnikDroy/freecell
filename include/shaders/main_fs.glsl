#version 330 core

out vec4 FragColor;

in vec4 outColor;
in vec2 tex_coords;

uniform sampler2D spritesheet;

void main()
{
    vec4 texColor = texture(spritesheet, tex_coords);
    FragColor = texColor * outColor;
}
