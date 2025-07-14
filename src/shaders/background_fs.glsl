#version 330 core

in vec2 world_pos;
out vec4 FragColor;

float rand(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = rand(i);
    float b = rand(i + vec2(1, 0));
    float c = rand(i + vec2(0, 1));
    float d = rand(i + vec2(1, 1));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float val = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 5; i++) {
        val += amp * smoothNoise(p * freq);
        freq *= 2.0;
        amp *= 0.5;
    }
    return val;
}

void main() {
    vec2 p = world_pos * 0.1; // scale for noise density
    float noise = fbm(p);

    vec3 base_green = vec3(0.1, 0.28, 0.16);
    vec3 color = base_green + noise * 0.08;

    FragColor = vec4(color, 1.0);
}
