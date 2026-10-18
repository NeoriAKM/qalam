#include "inc/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

void Config::load() {
    const char* home = std::getenv("HOME");
    if (!home) return;

    std::string config_path = std::string(home) + "/.config/qalam/config.json";
    std::ifstream in(config_path);

    nlohmann::json config_json;

    if (!in.is_open()) {
        std::cerr << "Config not found: " << config_path << '\n';
        return;
    }

    try {
        in >> config_json;

        font_path = config_json.value("font", font_path);
        font_size = config_json.value("font_size", font_size);
        theme = config_json.value("theme", theme);
        w_width  = config_json.value("w_width" , w_width);
        w_height = config_json.value("w_height", w_height);
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << '\n';
    }
}