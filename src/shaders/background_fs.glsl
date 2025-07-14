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
    vec2 p = world_pos * 0.1;

    vec3 baseColor = vec3(0.4, 0.05, 0.08); // deep red velvet

    float noise = fbm(p);
    float fine = smoothNoise(p * 8.0);
    float textureMix = mix(noise, fine, 0.2);
    vec3 noiseTint = vec3(0.6, 0.1, 0.15) * textureMix * 0.1;

    // Aspect-aware vignette
    vec2 centered = (world_pos - vec2(0.5)) * vec2(1.777, 1.0);
    float vignette = smoothstep(0.6, 0.95, length(centered));
    float glow = 1.0 - vignette;
    vec3 glowColor = vec3(0.7, 0.15, 0.2) * glow * 0.2;

    // Fake directional shimmer (like velvet highlights)
    vec2 lightDir = normalize(vec2(-1.0, -0.6));
    float directionalHighlight = dot(normalize(centered), lightDir);
    directionalHighlight = smoothstep(0.0, 1.0, directionalHighlight) * 0.05;
    vec3 shimmer = vec3(0.8, 0.2, 0.3) * directionalHighlight;

    vec3 finalColor = baseColor + noiseTint + glowColor + shimmer;

    // Slight desaturation for blend with UI
    finalColor = mix(finalColor, vec3(dot(finalColor, vec3(0.299, 0.587, 0.114))), 0.05);

    FragColor = vec4(finalColor, 1.0);
}

