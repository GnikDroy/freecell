#version 330 core

in vec2 world_pos;
out vec4 FragColor;

float rand(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
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
    float amp = 1.5;
    float freq = 5.0;
    for (int i = 0; i < 5; i++) {
        val += amp * smoothNoise(p * freq);
        freq *= 2.0;
        amp *= 0.5;
    }
    return val;
}

void main() {
    vec2 p = world_pos * 0.1;
    vec3 baseColor = vec3(20/256.0, 63/256.0, 23/256.0);

    float noise = fbm(p);
    float fine = smoothNoise(p * 8.0);
    float textureMix = mix(noise, fine, 0.2);
    vec3 noiseTint = vec3(1.0f, 1.0f, 1.0f) * textureMix * 0.05;

    vec3 finalColor = baseColor + noiseTint;

    FragColor = vec4(finalColor, 1.0);
}

