#version 300 es
precision mediump float;

in vec4 outColor;
in vec2 tex_coords;

uniform sampler2D spritesheet;

out vec4 FragColor;

void main() {
    vec4 texColor = vec4(1.0);

    // GLSL ES doesn't have isinf/isnan built-in, so we need a workaround

    // Check if tex_coords are valid (not NaN or Inf) by a safe clamp trick
    // NaN and Inf cause comparisons to fail, so this is a common check:
    if (all(greaterThanEqual(tex_coords, vec2(0.0))) && all(lessThanEqual(tex_coords, vec2(1.0)))) {
        texColor = texture(spritesheet, tex_coords);
    }

    if (texColor.a < 0.1) {
        discard;
    }

    FragColor = texColor * outColor;
}
