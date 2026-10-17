#pragma once

#include <string>

struct Config {
    std::string font_path = "/usr/share/fonts/TTF/DejaVuSansMono.ttf";
    int font_size = 20;
    std::string theme = "dark";
    bool debug_mode = false;

    void load();
};