#pragma once
#include "game.h"
#include "rect.h"
#include "sprite.h"

typedef enum CardUIState {
    CARD_UI_STATE_NORMAL,
    CARD_UI_STATE_HOVERED,
    CARD_UI_STATE_SELECTED,
} CardUIState;

typedef enum UIType {
    UI_CARD,
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
    int id;
    ButtonUIState state;
} ButtonUIMeta;

typedef struct UIElement {
    UIType type;
    Sprite sprite;
    Rect hitbox;
    union {
        CardUIMeta card;
        ButtonUIMeta button;
    } meta;
} UIElement;

void ui_element_apply_state_style(UIElement* ui_element);
