#pragma once
#include "core/aalloc.h"

#include "game/game.h"

#include "rendering/rect.h"
#include "rendering/sprite.h"

typedef enum CardUIState {
    CARD_UI_STATE_NORMAL,
    CARD_UI_STATE_HOVERED,
    CARD_UI_STATE_SELECTED,
    CARD_UI_STATE_DROP_TARGET,
} CardUIState;

typedef enum UIType {
    UI_CARD,
    UI_CARD_PLACEHOLDER,
    UI_TEXT,
    UI_BUTTON,
} UIType;

typedef struct CardUIMeta {
    Card card;
    SelectionLocation selection_location;
    int card_index;
    CardUIState state;
} CardUIMeta;

typedef enum ButtonUIState {
    BUTTON_UI_STATE_NORMAL,
    BUTTON_UI_STATE_HOVERED,
    BUTTON_UI_STATE_SELECTED,
    BUTTON_UI_STATE_DISABLED,
} ButtonUIState;

typedef struct ButtonUIMeta {
    APtr id;
    ButtonUIState state;
} ButtonUIMeta;

typedef struct TextUIMeta {
    APtr text;
    float font_scaling;
    float line_height_scaling;
    float character_spacing_scaling;
} TextUIMeta;

typedef struct UIElement {
    UIType type;
    Sprite sprite;
    Rect hitbox;
    union {
        CardUIMeta card;
        ButtonUIMeta button;
        TextUIMeta text;
    } meta;
} UIElement;
