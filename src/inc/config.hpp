#pragma once

#include <string>

struct Config {
    std::string font_path = "/usr/share/fonts/TTF/DejaVuSansMono.ttf"; // Arch linux path
    std::string theme = "dark";
    unsigned int font_size = 20;
    unsigned int fps = 60;

    int w_width  = 1000;
    int w_height = 600 ;

    std::string placeholder = "Qalam. Live long and prosper";
    std::string unnamed = "unnamed.txt";

    void load();
};