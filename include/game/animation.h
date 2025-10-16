#pragma once

#include "core/vector.h"
#include "game/ui_element.h"

typedef struct Controller Controller;

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

AnimationSystem animation_system_init(void);

void animation_system_free(AnimationSystem* system);

UIElement animation_system_get_next_frame(
    AnimationSystem* system,
    UIElementAnimation* animation
);

void animation_system_update(AnimationSystem* system, Controller* controller, float delta_time);

bool animation_system_is_animated(AnimationSystem* anim_sys, UIElement* element);
