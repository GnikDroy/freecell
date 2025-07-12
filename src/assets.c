#include "assets.h"
#include "card_sprites.h"

const char MAIN_SPRITESHEET[] = {
#embed "assets/spritesheet.png"
};

Assets assets_init() {
  Assets assets;
  assets.spritesheet = image_load_from_memory((const uint8_t *)MAIN_SPRITESHEET,
                                              sizeof(MAIN_SPRITESHEET), 4);
  generate_card_sprites(assets.deck);
  return assets;
}

void assets_free(Assets *assets) { image_free(&assets->spritesheet); }
