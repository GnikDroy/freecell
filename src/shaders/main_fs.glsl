#version 330 core

out vec4 FragColor;

in vec4 outColor;
in vec2 tex_coords;

uniform sampler2D spritesheet;

void main()
{
    vec4 texColor = texture(spritesheet, tex_coords);

    if (texColor.a < 0.1)
        discard;

    const vec2 cardSize = vec2(1.0 / 14.0, 1.0 / 4.0);
    vec2 cardOrigin = floor(tex_coords / cardSize) * cardSize;
    vec2 localUV = (tex_coords - cardOrigin) / cardSize;

    vec2 centered = localUV - vec2(0.5);

    // Subtle vignette
    float vignette = smoothstep(0.9, 0.6, length(centered));

    // Border glow
    vec2 edgeDist = min(localUV, vec2(1.0) - localUV);
    float border = min(edgeDist.x, edgeDist.y);
    float edgeGlow = smoothstep(0.08, 0.02, border);  // high when near edge

    // Final color
    vec3 goldenColor = vec3(1.0, 0.9, 0.6); // light golden tint
    vec3 goldenTint = mix(vec3(1.0), goldenColor, 0.50); // 25% golden

    vec3 baseColor = texColor.rgb * outColor.rgb * goldenTint * vignette;

    // Procedural paper-like texture based on UV
    float grain = sin(localUV.x * 300.0) * sin(localUV.y * 300.0);

    grain = grain * 0.5 + 0.5; // Normalize to [0,1]
    grain = mix(1.0, grain, 0.15); // 25% strength

    baseColor *= grain;

    vec2 lightDir = normalize(vec2(-1.0, -1.0)); // Fake light from top-left
    float bevel = dot(normalize(centered), lightDir);
    bevel = smoothstep(-1.0, 1.0, bevel);
    bevel = mix(0.95, 1.05, bevel); // Slight bump

    float highlight = smoothstep(0.4, 0.0, length(centered)) * 0.05;
    baseColor += vec3(highlight);

    baseColor += vec3(0.10, 0.10, 0.03) * edgeGlow;
    FragColor = vec4(baseColor, texColor.a * outColor.a);
}

