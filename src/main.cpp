// Qalam - Text editor v1.1
// Copyright NeoriAKM(c)

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shape.hpp>

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#include "inc/saving.hpp"
#include "inc/config.hpp"

//#include <cstdlib>


/*
std::vector<std::string> fonts = {
    "etc/font.ttf",                                        // classic
    "/usr/share/fonts/TTF/DejaVuSansMono.ttf",             // Arch
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", // Debian
    };
    Coming soon
*/


static size_t buflen(int line, const std::vector<std::string>& lines) {return (int)lines[line].size();}

//////////////////////////////////////////////////////////////////
//////////////////////// MAIN DEFINITON //////////////////////////
//////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    Config config;
    config.load();

    bool debug_mode = false;

    std::vector<std::string> lines = {};
    std::string placeholder = "Qalam. Live long and prosper!";
    std::string save_filename = "unnamed.txt";
    
    sf::Color theme_color1 = sf::Color::White; // fg
    sf::Color theme_color2 = sf::Color::Black; // bg
    
    for (int i=0; i<argc; i++) {
        if (i == 0) continue;
        
        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::cout << "+-------------------- help -------------------+\n";
            std::cout << "| left  click to button - quick save          |\n";
            std::cout << "| right click to button - choose path to save |\n";
            std::cout << "+---------------------------------------------+\n";
            std::cout << "| qalam <file>  -  open this file with qalam  |\n";
            std::cout << "+---------------------------------------------+\n";
            std::cout << "| --help  -h       - opening this reference   |\n";
            std::cout << "| --debug -d       - turning on debug mode    |\n";
            std::cout << "+---------------------------------------------+\n";
            return 0;
        } else if (std::strcmp(argv[i], "--debug") == 0 || std::strcmp(argv[i], "-d") == 0) {
            debug_mode = true;
        } else if (argv[i][0] != '-') {
            save_filename = argv[i];
        }
    }

    /////////////// config //////////////////

    std::string font_path = config.font_path;
    int font_size = config.font_size;
    std::string theme = config.theme;
    
    if (debug_mode) {
        std::cout << "Font: " << font_path << '\n';
        std::cout << "Size: " << font_size << '\n';
        std::cout << "Theme: "<< theme     << '\n';
    }


    if (theme == "light") {
        theme_color1 = sf::Color::Black;
        theme_color2 = sf::Color::White;
    }

    ///////////////////////////////////////////////

    std::ifstream in(save_filename);

    if (in.is_open()) {
        std::string line;
        while (std::getline(in, line)) {
            lines.push_back(line);
        }
        in.close();
    }
    if (lines.empty()) {
        lines.push_back(placeholder);
    }

    sf::Vector2f windowSize(config.w_width, config.w_height);

    sf::RenderWindow window(
        sf::VideoMode({(unsigned int)windowSize.x, (unsigned int)windowSize.y}),
        sf::String("Qalam: '") + sf::String(save_filename) + "'"
    );

    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile(font_path)) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return 1;
    }
    
    const sf::Glyph& glyph = font.getGlyph('M', font_size, false);
    float charw = glyph.advance;
    
    int cursorY = (int)lines.size()-1;
    int cursorX = buflen(cursorY, lines);

    int offsetY = 0;
    int offsetX = 0;

    const float line_height = font.getLineSpacing(font_size);
    const float text_x = 20.f;
    const float text_y = 20.f;

    sf::Text text(font, lines[cursorY], font_size);
    sf::Text save_label(font, "Save", 25);
    sf::RectangleShape save_button(sf::Vector2f(100, 40));
    sf::RectangleShape cursor({2.f, line_height});
    
    save_button.setPosition(sf::Vector2f(windowSize.x-120, windowSize.y-60));
    save_label.setPosition({windowSize.x - 110.f, windowSize.y - 60.f});
    text.setPosition({20.f, 0.f});
    
    text.setFillColor(theme_color1);
    save_button.setFillColor(sf::Color::Blue);
    save_label.setFillColor(sf::Color::White);
    cursor.setFillColor(theme_color1);


    ////////////////////// MAINLOOP ///////////////////////////

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                if (debug_mode) for (int i=0; i<(int)lines.size(); i++) {std::cout << lines[i] << '\n';}
                window.close();
            }

            // ========== Left Right keys ==========

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::Left) {
                    if (cursorX > 0) cursorX--;
                    else if (cursorY > 0) {
                        cursorY--;
                        cursorX = buflen(cursorY, lines);
                    }
                }
                if (key->code==sf::Keyboard::Key::Right) {
                    if (cursorX < buflen(cursorY, lines))
                        cursorX++;
                    else if (cursorY != (int)lines.size()-1) {
                        cursorX=0; 
                        cursorY++;
                    }
                }

                // ========== Up-Down keys ==========

                if (key->code == sf::Keyboard::Key::Down && cursorY < (int)lines.size()-1) {
                    cursorY++;
                    if (cursorX > buflen(cursorY, lines)) {
                        cursorX = buflen(cursorY, lines);
                    }
                }
                if (key->code==sf::Keyboard::Key::Up) {
                    if (cursorY > 0) {
                        cursorY--;
                        if (cursorX > buflen(cursorY, lines)) {
                            cursorX = buflen(cursorY, lines);
                        }
                    }
                }
                
                // ========== Delete keys ==========

                if (key->code == sf::Keyboard::Key::Backspace) {
                    if (cursorX > 0) {
                        lines[cursorY].erase(cursorX - 1, 1);
                        cursorX--;
                    } else if (cursorY > 0) {
                        int old_len = buflen(cursorY - 1, lines);
                        lines[cursorY - 1] += lines[cursorY];
                        lines.erase(lines.begin() + cursorY);
                        cursorY--;
                        cursorX = old_len;
                    }
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < buflen(cursorY, lines)) {
                    lines[cursorY].erase(cursorX, 1);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    for (int i = 0; i < 4; i++) {
                        int spaces = 4 - (cursorX % 4);
                        for (int i = 0; i < spaces; i++)
                        {lines[cursorY].insert(cursorX, 1, ' '); cursorX++;}
                        cursorX++;
                    }
                }
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::string rest = lines[cursorY].substr(cursorX);
                    lines[cursorY].erase(cursorX);
                    lines.insert(lines.begin() + cursorY + 1, rest);
                    cursorY++;
                    cursorX=0;
                }
                if (key->code == sf::Keyboard::Key::S && key->control) {
                    save(lines, save_filename);
                }
                if (key->code == sf::Keyboard::Key::S && key->control && key->shift) {
                    make_save_window(save_filename, lines);
                }
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    lines[cursorY].insert(cursorX, 1, static_cast<char>(t->unicode));
                    cursorX++;
                }
            }

            // Save button Click
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos((float)mouse->position.x, (float)mouse->position.y);
                    if (save_button.getGlobalBounds().contains(pos)) {
                        save(lines, save_filename);
                        save_button.setFillColor(sf::Color::Cyan);
                    }
                } else if (mouse->button == sf::Mouse::Button::Right) {
                    sf::Vector2f pos((float)mouse->position.x, (float)mouse->position.y);
                    if (save_button.getGlobalBounds().contains(pos)) {
                        make_save_window(save_filename, lines);
                        save_button.setFillColor(sf::Color::Cyan);
                    }
                }
            } else save_button.setFillColor(sf::Color::Blue);

            /*
            
                MOUSE SCROLL PART
            
            */
            if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>()) {
    
                bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

                if (mouse->wheel == sf::Mouse::Wheel::Vertical) {
                    if (shift_pressed) {
                        offsetX -= (int)(mouse->delta * charw * 5);

                        int max_line_length = 0;
                        for(const auto&line:lines) {
                            if((int)line.size()>max_line_length)max_line_length=line.size();
                        }
                        float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                        if (offsetX < 0) offsetX = 0;
                        if (offsetX > max_scroll) offsetX = max_scroll;

                    } else {
                        // Vertical
                        offsetY -= (int)(mouse->delta * line_height * 3);
                        float max_scroll = std::max(0.f, lines.size() * line_height - windowSize.y + 60.f);
                        if (offsetY < 0) offsetY = 0;
                        if (offsetY > max_scroll) offsetY = max_scroll;
                    }
                } 
                // Horizontal toychpad
                else if (mouse->wheel == sf::Mouse::Wheel::Horizontal) {
                    offsetX -= (int)(mouse->delta * charw * 5);

                    int max_line_length = 0;
                    for(const auto&line:lines)if((int)line.size()>max_line_length)max_line_length=line.size();
                    float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                    if (offsetX < 0) offsetX = 0;
                    if (offsetX > max_scroll) offsetX = max_scroll;
                }
            }
        }
        cursor.setPosition({text_x + cursorX * charw - offsetX, text_y + cursorY * line_height - offsetY});

        if (theme == "dark") window.clear(sf::Color(30, 30, 30));
        else window.clear(sf::Color(245, 245, 220));

        for (int i = 0; i < (int)lines.size(); i++) {
            text.setString(lines[i]);
            text.setPosition({text_x - offsetX, text_y + i * line_height - offsetY});
            window.draw(text);
        }
        window.draw(cursor);
        window.draw(save_button);
        window.draw(save_label);

        window.display();
    }
}