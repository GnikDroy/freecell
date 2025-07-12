#pragma once

const char GAME_TITLE[] = "Freecell";
const int GAME_MIN_WIDTH = 800;
const int GAME_MIN_HEIGHT = 600;

const int VIRTUAL_WIDTH = 1100;
const int VIRTUAL_HEIGHT = 800;

const char MAIN_VERTEX_SHADER_SOURCE[] = {
#embed "shaders/main_vs.glsl"
    , '\0'};

const char MAIN_FRAGMENT_SHADER_SOURCE[] = {
#embed "shaders/main_fs.glsl"
    , '\0'};

