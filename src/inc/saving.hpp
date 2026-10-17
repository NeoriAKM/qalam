#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

void save(const std::vector<std::string>& list, std::string filename);

void handle_save_window_event(
    const sf::Event& event,
    sf::RenderWindow& window,
    std::string& filename,
    int& cursorX,
    const std::vector<std::string>& lines
);

void make_save_window(std::string save_filename, const std::vector<std::string>& lines);