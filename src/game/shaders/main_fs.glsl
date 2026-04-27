#version 330 core

out vec4 FragColor;

in vec4 outColor;
in vec2 tex_coords;

uniform sampler2D spritesheet;

void main()
{
    vec4 texColor = vec4(1.0);

    if (tex_coords.x == tex_coords.x && tex_coords.y == tex_coords.y && tex_coords.x >= 0.0
        && tex_coords.x <= 1.0 && tex_coords.y >= 0.0 && tex_coords.y <= 1.0) {
        texColor = texture(spritesheet, tex_coords);
    }

    if (texColor.a < 0.1)
        discard;

    FragColor = texColor * outColor;
}

