#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include "inc/helper.hpp"

using namespace std;

void help_msg() {
    cout << "+-------------------- help msg ----------------------+\n";
    cout << "| left  click to button - quick save                 |\n";
    cout << "| right click to button - choose path to save        |\n";
    cout << "|                                                    |\n";
    cout << "|          read about config in README.md            |\n";
    cout << "+----------------------------------------------------+\n";
    cout << "| qalam <file>  -  open this file with qalam         |\n";
    cout << "+-------------------- hotkeys -----------------------+\n";
    cout << "| Ctrl+S        -  quick save                        |\n";
    cout << "| Ctrl+Shift+S  -  save with path choice             |\n";
    cout << "|                                                    |\n";
    cout << "| Ctrl+C        -  copying string under cursor       |\n";
    cout << "| Ctrl+Shift+C  -  copying ALL file to clipbaord     |\n";
    cout << "| Ctrl+V        -  paste text from your clipboard    |\n";
    cout << "| Ctrl+D        -  copy string under cursor to next  |\n";
    cout << "|                                                    |\n";
    cout << "| Ctrl+U        -  moving cursor to center of screen |\n";
    cout << "+---------------------- flags -----------------------+\n";
    cout << "| --help    -h        - opening this reference       |\n";
    cout << "| --version -v        - opening copyright text       |\n";
    cout << "| --debug   -d <file> - turning on debug mode        |\n";
    cout << "| --nomods  -m <file> - turn off mods launch         |\n";
    cout << "+----------------------------------------------------+\n\n";
    cout << "Good luck for your future projects with(out) Qalam!\n";
    return;
}

void copyright_msg() {
    cout << "+-----------------------------------+\n";
    cout << "|     Copyright © NeoriAKM (2026)   |\n";
    cout << "| All right reserved by MIT license |\n";
    cout << "|        More in file LICENSE       |\n";
    cout << "+-----------------------------------+\n";
    return;
}

size_t buflen(size_t line, const vector<string>& lines) {return lines[line].size();}


string repeat(const string& str, size_t times) {
    string result;
    result.reserve(str.length() * times);
    for (size_t i = 0; i < times; ++i) {
        result += str;
    }
    return result;
}

void copy_string(vector<string>& lines, size_t strnum) {
    if (lines.empty()) return;
    sf::Clipboard::setString(sf::String(lines[strnum]));
}

void copy_vec(vector<string>& lines) {
    if (lines.empty()) return;

    string result = accumulate(
        next(lines.begin()), lines.end(), lines[0],
        [](const string& a, const string& b) {return a + "\n" + b;}
    );

    sf::Clipboard::setString(sf::String(result));
}

void update_visible(Editor& editor, float line_height, float window_height) {
    editor.first_visible = static_cast<size_t>(std::max(0.f, editor.offsetY / line_height));
    size_t last = static_cast<size_t>(std::max(0.f, (editor.offsetY + window_height) / line_height)) + 1;
    editor.last_visible = std::min(editor.lines.size() - 1, last);
}

void handle_arrows(Editor& editor, sf::Keyboard::Key code, float line_height, sf::Vector2f windowSize) {

    if (code==sf::Keyboard::Key::Left) {
        if (editor.cursorX > 0) editor.cursorX--;
        else if (editor.cursorY > 0) {
            editor.cursorY--;
            editor.cursorX = buflen(editor.cursorY, editor.lines);
        }
    }
    if (code==sf::Keyboard::Key::Right) {
        if (editor.cursorX < buflen(editor.cursorY, editor.lines))
            editor.cursorX++;
        else if (editor.cursorY != editor.lines.size()-1) {
            editor.cursorX=0;
            editor.cursorY++;
        }
    }

    // ========== Up-Down keys ==========
    float max_scroll = std::max(0.f, editor.lines.size() * line_height - windowSize.y + 100.f);

    if (code == sf::Keyboard::Key::Down && editor.cursorY < editor.lines.size()-1) {
        editor.cursorY++;

        if (editor.cursorY > editor.last_visible)  editor.offsetY += line_height;

        if (editor.offsetY > max_scroll) editor.offsetY = max_scroll;
        if (editor.cursorX > buflen(editor.cursorY, editor.lines)) {
            editor.cursorX = buflen(editor.cursorY, editor.lines);
        }
    }
    if (code == sf::Keyboard::Key::Up) {
        if (editor.cursorY > 0) {
            editor.cursorY--;

            if (editor.cursorY < editor.first_visible) editor.offsetY -= line_height;

            if (editor.offsetY < 0) editor.offsetY = 0;
            if (editor.cursorX > buflen(editor.cursorY, editor.lines)) {
                editor.cursorX = buflen(editor.cursorY, editor.lines);
            }
        }
    }
}

void handle_backspaces(
    Editor& editor, float line_height, const sf::Event::KeyPressed* key,
    sf::Vector2f windowSize, std::string placeholder, int w_height
) {
    float max_scroll = std::max(0.f, editor.lines.size() * line_height - windowSize.y + 100.f);

    if (key->code == sf::Keyboard::Key::Backspace) {
        if (key->control) {
            while (editor.cursorX > 0 && editor.lines[editor.cursorY][editor.cursorX - 1] == ' ') editor.cursorX--;
            while (editor.cursorX > 0 && editor.lines[editor.cursorY][editor.cursorX - 1] != ' ') {
                editor.lines[editor.cursorY].erase(editor.cursorX - 1, 1);
                editor.cursorX--;
            }
        } else { // simple backspace

            // Placeholder removing
            if (editor.lines[editor.cursorY] == placeholder) {editor.lines[editor.cursorY] = ""; editor.cursorX = 0;}

            if (editor.cursorX > 0) {
                editor.lines[editor.cursorY].erase(editor.cursorX - 1, 1);
                editor.cursorX--;
            } else if (editor.cursorY > 0) {
                size_t old_len = buflen(editor.cursorY - 1, editor.lines);
                editor.lines[editor.cursorY - 1] += editor.lines[editor.cursorY];
                editor.lines.erase(editor.lines.begin() + static_cast<ptrdiff_t>(editor.cursorY));
                editor.cursorY--;
                editor.cursorX = old_len;

                if (editor.cursorY >= editor.lines.size()) editor.cursorY = editor.lines.size() - 1;
                if (editor.cursorX > buflen(editor.cursorY, editor.lines))
                    editor.cursorX = buflen(editor.cursorY, editor.lines);

                if (editor.offsetY > max_scroll) editor.offsetY = (int)max_scroll;
                if (editor.offsetY < 0) editor.offsetY = 0;

                update_visible(editor, line_height, w_height);
            }
        }
    }
    if (key->code == sf::Keyboard::Key::Delete && editor.cursorX < buflen(editor.cursorY, editor.lines)) {
        editor.lines[editor.cursorY].erase(editor.cursorX, 1);
    }
}

void handle_tab(Editor& editor, sf::Keyboard::Key code) {
    if (code == sf::Keyboard::Key::Tab) {
        int spaces = 4 - (editor.cursorX % 4);
        for (int i = 0; i < spaces; i++) {
            editor.lines[editor.cursorY].insert(editor.cursorX, 1, ' ');
            editor.cursorX++;
        }
    }
}


void handle_enter(Editor& editor, sf::Keyboard::Key code) {
    if (code == sf::Keyboard::Key::Enter) {
        std::string rest = editor.lines[editor.cursorY].substr(editor.cursorX);
        editor.lines[editor.cursorY].erase(editor.cursorX);

        size_t indent = 0;
        while (indent < editor.lines[editor.cursorY].size() && editor.lines[editor.cursorY][indent] == ' ') indent++;

        editor.lines.insert(editor.lines.begin() + static_cast<std::ptrdiff_t>(editor.cursorY+1), (std::string(indent, ' ') + rest));
        editor.cursorY++;
        editor.cursorX = indent;
    }
}


void handle_hotkeys(
    Editor& editor, const sf::Event::KeyPressed* key, float line_height
) {


    // ! CURSOR ACTS
    if (key->code == sf::Keyboard::Key::U && key->control && !key->shift) { // ctrl+U
        editor.cursorY = (editor.last_visible + editor.first_visible) / 2;
        editor.cursorX = buflen(editor.cursorY, editor.lines);
    }

    // ! SAVES
    if (key->code == sf::Keyboard::Key::S && key->control && !key->shift) { // ctrl+S
        save(editor.lines, editor.save_filename);
    }
    if (key->code == sf::Keyboard::Key::S && key->control && key->shift) {  // Ctrl+Shift+S
        make_save_window(editor.save_filename, editor.lines);
    }

    // ! COPIES
    if (key->code == sf::Keyboard::Key::C && key->control && !key->shift) {  // Ctrl+C
        copy_string(editor.lines, editor.cursorY);
    }
    if (key->code == sf::Keyboard::Key::C && key->control && key->shift)  { // Ctrl+Shift+C
        copy_vec(editor.lines);
    }
    if (key->code == sf::Keyboard::Key::V && key->control && !key->shift) { // Ctrl+V
        const string clipboard = sf::Clipboard::getString().toAnsiString();
        editor.lines[editor.cursorY].insert(editor.cursorX, clipboard);
        editor.cursorX += clipboard.size();
    }
    if (key->code == sf::Keyboard::Key::D && key->control && !key->shift) { // Ctrl+D
        string being_copied = editor.lines[editor.cursorY];
        editor.lines.insert(editor.lines.begin()+static_cast<long>(editor.cursorY)+1, being_copied);

        editor.cursorY++;
        if (editor.cursorY > editor.last_visible) editor.offsetY += line_height;
    }
}