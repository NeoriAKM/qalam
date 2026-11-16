#pragma once

#include <string>
#include <cstdint>
#include <array>

struct Config {
    std::string font_path = "/usr/share/fonts/TTF/DejaVuSansMono.ttf"; // Arch linux path
    unsigned int font_size = 20;
    unsigned int fps = 60;

    int w_width  = 1000;
    int w_height = 600 ;

    std::string placeholder = "Qalam. Live long and prosper";
    std::string unnamed = "unnamed.txt";

    std::array<uint8_t, 3> fg_c = {50,  50,  50 };
    std::array<uint8_t, 3> bg_c = {255, 255, 255};

    void load();
};