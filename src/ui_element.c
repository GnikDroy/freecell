#include "ui_element.h"

void ui_element_apply_state_style(UIElement *ui_element) {
  if (ui_element->type == UI_CARD) {
    if (ui_element->meta.card.state == CARD_UI_STATE_SELECTED) {
      ui_element->sprite.color.r = 0.6f;
      ui_element->sprite.color.g = 0.6f;
      ui_element->sprite.color.b = 0.6f;
    } else if (ui_element->meta.card.state == CARD_UI_STATE_HOVERED) {
      ui_element->sprite.color.r = 0.8f;
      ui_element->sprite.color.g = 0.8f;
      ui_element->sprite.color.b = 0.8f;
    }
  }
}
