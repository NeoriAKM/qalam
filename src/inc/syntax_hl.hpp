#pragma once
#include <string>
#include <SFML/Graphics/Color.hpp>

std::string get_ext(std::string filename);

bool is_comment(std::string line, std::string filename);
bool is_label(std::string line, std::string filename);
sf::Color cmd_asm(std::string line, std::string filename, sf::Color non);
bool is_macro(std::string line, std::string filename);