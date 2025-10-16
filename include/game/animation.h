#pragma once

#include "core/vector.h"
#include "game/ui_element.h"
#include "platform/window.h"
#include "utils.h"

typedef enum UIElementAnimationEndBehaviour {
    ANIMATION_DELETE_ON_FINISH,
    ANIMATION_STOP_ON_FINISH,
    ANIMATION_LOOP,
} UIElementAnimationEndBehaviour;

typedef struct UIElementAnimation {
    UIElement from;
    UIElement to;

    float elapsed;
    float duration;
    UIElementAnimationEndBehaviour behaviour;
} UIElementAnimation;

typedef struct AnimationSystem {
    Vector ui_animations;
} AnimationSystem;

inline AnimationSystem animation_system_init(void) {
    AnimationSystem system = {
        .ui_animations = vec_init(sizeof(UIElementAnimation)),
    };
    return system;
}

inline void animation_system_free(AnimationSystem* system) { vec_free(&system->ui_animations); }

inline UIElement animation_system_get_next_frame(
    AnimationSystem* system,
    UIElementAnimation* animation
) {
    UIElement result = animation->from;

    float factor = (animation->duration > 0.0f) ? animation->elapsed / animation->duration : 1.0f;
    factor = ease_in_out_cubic(factor);

    result.sprite.x = lerp(animation->from.sprite.x, animation->to.sprite.x, factor);
    result.sprite.y = lerp(animation->from.sprite.y, animation->to.sprite.y, factor);
    result.sprite.z = lerp(animation->from.sprite.z, animation->to.sprite.z, factor);
    result.sprite.rotation
        = lerp(animation->from.sprite.rotation, animation->to.sprite.rotation, factor);

    result.sprite.color.r
        = lerp(animation->from.sprite.color.r, animation->to.sprite.color.r, factor);
    result.sprite.color.g
        = lerp(animation->from.sprite.color.g, animation->to.sprite.color.g, factor);
    result.sprite.color.b
        = lerp(animation->from.sprite.color.b, animation->to.sprite.color.b, factor);
    result.sprite.color.a
        = lerp(animation->from.sprite.color.a, animation->to.sprite.color.a, factor);

    result.sprite.width = lerp(animation->from.sprite.width, animation->to.sprite.width, factor);
    result.sprite.height = lerp(animation->from.sprite.height, animation->to.sprite.height, factor);

    result.hitbox.x = lerp(animation->from.hitbox.x, animation->to.hitbox.x, factor);
    result.hitbox.y = lerp(animation->from.hitbox.y, animation->to.hitbox.y, factor);
    result.hitbox.width = lerp(animation->from.hitbox.width, animation->to.hitbox.width, factor);
    result.hitbox.height = lerp(animation->from.hitbox.height, animation->to.hitbox.height, factor);

    if (result.type == UI_CARD) {
        result.meta.card.state = CARD_UI_STATE_NORMAL;
    }

    return result;
}

inline void animation_system_update(AnimationSystem* system, float delta_time) {
    for (size_t i = 0; i < system->ui_animations.size;) {
        vec_get_as(UIElementAnimation, animation, &system->ui_animations, i);
        animation.elapsed = clamp(animation.elapsed + delta_time, -INFINITY, animation.duration);
        vec_set(&system->ui_animations, i, &animation);

        if (animation.elapsed >= animation.duration) {
            if (animation.behaviour == ANIMATION_DELETE_ON_FINISH) {
                vec_delete(&system->ui_animations, i);
            } else if (animation.behaviour == ANIMATION_STOP_ON_FINISH) {
                i++;
            } else {
                animation.elapsed = 0.0f;
                i++;
            }
        } else {
            i++;
        }
    }
}

inline bool animation_system_is_animated(AnimationSystem* anim_sys, UIElement* element) {
    for (size_t i = 0; i < anim_sys->ui_animations.size; i++) {
        vec_get_as(UIElementAnimation, anim, &anim_sys->ui_animations, i);
        if (anim.to.type == element->type && element->type == UI_CARD) {
            if (anim.to.meta.card.selection_location == element->meta.card.selection_location
                && anim.to.meta.card.card_index == element->meta.card.card_index) {
                return true;
            }
        }

        if (anim.from.type == element->type && element->type == UI_CARD) {
            if (anim.from.meta.card.selection_location == element->meta.card.selection_location
                && anim.from.meta.card.card_index == element->meta.card.card_index) {
                return true;
            }
        }
    }
    return false;
}
