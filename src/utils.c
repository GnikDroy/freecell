#include "utils.h"
#include "constants.h"
#include <string.h>

bool point_in_rect(float px, float py, Rect rect) {
    float halfW = rect.width / 2.0f;
    float halfH = rect.height / 2.0f;

    return px >= rect.x - halfW && px <= rect.x + halfW && py >= rect.y - halfH
        && py <= rect.y + halfH;
}

vec2s screen_to_world(
    double mouse_x, double mouse_y, int screen_width, int screen_height, Camera* cam) {
    // First, compute the letterboxed viewport
    float aspect_virtual = (float)VIRTUAL_WIDTH / VIRTUAL_HEIGHT;
    float aspect_window = (float)screen_width / screen_height;

    int viewport_width, viewport_height;
    int viewport_x, viewport_y;

    if (aspect_window > aspect_virtual) {
        viewport_height = screen_height;
        viewport_width = (int)(viewport_height * aspect_virtual);
    } else {
        viewport_width = screen_width;
        viewport_height = (int)(viewport_width / aspect_virtual);
    }

    viewport_x = (screen_width - viewport_width) / 2;
    viewport_y = (screen_height - viewport_height) / 2;

    // Adjust mouse coordinates to remove letterboxing offset
    float adjusted_x = (float)(mouse_x - viewport_x);
    float adjusted_y = (float)(mouse_y - viewport_y);

    // Clamp to inside the viewport (optional safety)
    if (adjusted_x < 0)
        adjusted_x = 0;
    if (adjusted_x > viewport_width)
        adjusted_x = (float)viewport_width;
    if (adjusted_y < 0)
        adjusted_y = 0;
    if (adjusted_y > viewport_height)
        adjusted_y = (float)viewport_height;

    // Convert to NDC
    float ndc_x = (2.0f * adjusted_x) / viewport_width - 1.0f;
    float ndc_y = 1.0f - (2.0f * adjusted_y) / viewport_height;

    vec4 clip = { ndc_x, ndc_y, 0.0f, 1.0f };

    mat4 proj, view;
    memcpy(proj, cam->projection, sizeof(proj));
    memcpy(view, cam->view, sizeof(view));

    mat4 vp, inv_vp;
    glm_mat4_mul(proj, view, vp);
    glm_mat4_inv(vp, inv_vp);

    vec4 world;
    glm_mat4_mulv(inv_vp, clip, world);

    if (world[3] != 0.0f) {
        world[0] /= world[3];
        world[1] /= world[3];
    }

    return (vec2s) { .x = world[0], .y = world[1] };
}
