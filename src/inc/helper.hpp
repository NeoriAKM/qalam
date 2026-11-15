#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <vector>
#include "dialogues.hpp"

void help_msg();
void copyright_msg();
size_t buflen(size_t line, const std::vector<std::string>& lines);
std::string repeat(const std::string& str, size_t times);

void copy_string(std::vector<std::string>& lines, size_t strnum);
void copy_vec(std::vector<std::string>& lines);

struct Flags {
    bool debug = false;
    bool mods = true;
};

struct Args {
    bool debug = false;
    bool mods = true;
    std::string filename = "";
};

struct Editor {
    std::vector<std::string> lines;
    size_t cursorX = 0;
    size_t cursorY = 0;
    float offsetX = 0;
    float offsetY = 0;
    std::string save_filename;

    size_t first_visible;
    size_t last_visible;

    std::vector<Button> buttons;
};

inline Args parse_args(int argc, char* argv[]) {
    Args args;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            copyright_msg();
            exit(0);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help_msg();
            exit(0);
        } else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            args.debug = true;
        } else if (strcmp(argv[i], "--nomods") == 0 || strcmp(argv[i], "-m") == 0) {
            args.mods = false;
        } else if (argv[i][0] != '-') {
            args.filename = argv[i];
        }
    }
    return args;
}

void update_visible(Editor& editor, float line_height, float window_height);

////////////////// Input handles //////////////////

void handle_arrows(Editor& editor, sf::Keyboard::Key code, float line_height, sf::Vector2f windowSize);

void handle_backspaces(
    Editor& editor, float line_height,
    const sf::Event::KeyPressed* key, sf::Vector2f windowSize,
    std::string placeholder, int w_height
);

void handle_tab(Editor& editor, sf::Keyboard::Key code);

void handle_enter(Editor& editor, sf::Keyboard::Key code);

void handle_hotkeys(
    Editor& editor, const sf::Event::KeyPressed* key, float line_height
);