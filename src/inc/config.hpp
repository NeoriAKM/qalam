#pragma once

#include <string>

struct Config {
    std::string font_path = "/usr/share/fonts/TTF/DejaVuSansMono.ttf"; // Arch linux path
    int font_size = 20;
    std::string theme = "dark";

    int w_width  = 1000;
    int w_height = 600 ;

    void load();
};