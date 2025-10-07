#version 330 core

out vec4 FragColor;

in vec4 outColor;
in vec2 tex_coords;

uniform sampler2D spritesheet;

void main()
{
    vec4 texColor = vec4(1.0);

    if (!(isinf(tex_coords.x) || isnan(tex_coords.x))) {
        texColor = texture(spritesheet, tex_coords);
    }

    if (texColor.a < 0.1)
        discard;

    FragColor = texColor * outColor;
}

