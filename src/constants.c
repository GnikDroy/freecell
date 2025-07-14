#include "constants.h"

const char GAME_TITLE[] = "Freecell";
const int GAME_MIN_WIDTH = 800;
const int GAME_MIN_HEIGHT = 600;

const int VIRTUAL_WIDTH = 1600;
const int VIRTUAL_HEIGHT = 900;

const char MAIN_VERTEX_SHADER_SOURCE[] = {
#embed "shaders/main_vs.glsl"
    , '\0'
};

const char MAIN_FRAGMENT_SHADER_SOURCE[] = {
#embed "shaders/main_fs.glsl"
    , '\0'
};

const char BACKGROUND_VERTEX_SHADER_SOURCE[] = {
#embed "shaders/background_vs.glsl"
};

const char BACKGROUND_FRAGMENT_SHADER_SOURCE[] = {
#embed "shaders/background_fs.glsl"
};
