// Qalam - Text editor v1.0
// Copyright NeoriAKM(c)

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>

std::vector<std::string> lines = {};
std::string placeholder = "Qalam Text Editor. Live long and prosper!";
int offsetY = 0;
std::string save_filename = "etc/text.txt";

int buflen(int line) {return (int)lines[line].size();}
int bufc() {return (int)lines.size();}

/*-------------------------------------------------------*/

void save(const std::vector<std::string>& list, std::string filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Cannot open file for writing\n";
        return;
    }
    for (const auto& line : list) {
        out << line << '\n';
    }
}


void make_save_window() {
    sf::Vector2f windowSize(300.f, 100.f);

    sf::RenderWindow window(
        sf::VideoMode({
            (unsigned int)windowSize.x,
            (unsigned int)windowSize.y
        }),
        "Save window"
    );
    window.setFramerateLimit(30);

    sf::Font font;
    if (!font.openFromFile("etc/font.ttf")) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return;
    }

    int charsize = 15;
    
    const sf::Glyph& glyph = font.getGlyph('M', charsize, false);
    float charw = glyph.advance;
    
    int cursorX = 0;

    const float line_height = font.getLineSpacing(charsize);

    sf::Text text(font, save_filename, charsize);
    sf::RectangleShape cursor({2.f, line_height});

    cursor.setFillColor(sf::Color::White);
    text.setPosition({10.f, 5.f});
 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                // ========== Left Right keys ==========
                if (key->code==sf::Keyboard::Key::Left && cursorX > 0) cursorX--;
                if (key->code==sf::Keyboard::Key::Right && cursorX < (int)save_filename.size()) cursorX++;
                
                // ========== Up-Down keys ==========
                if (key->code == sf::Keyboard::Key::Down) cursorX = 0;
                if (key->code==sf::Keyboard::Key::Up) cursorX = (int)save_filename.size();
                
                // ========== Delete keys ==========
                if (key->code == sf::Keyboard::Key::Backspace && cursorX > 0) {
                    save_filename.erase(cursorX - 1, 1);
                    cursorX--;
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < (int)save_filename.size()) save_filename.erase(cursorX, 1);
                if (key->code == sf::Keyboard::Key::Enter) {
                    save(lines, save_filename);
                    window.close();
                }
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    save_filename.insert(cursorX, 1, static_cast<char>(t->unicode));
                    cursorX++;
                }
            }
        }
        text.setString(save_filename);
        cursor.setPosition({10.f + cursorX * charw, 5.f});
        window.clear(sf::Color(30, 30, 30));

        window.draw(text);
        window.draw(cursor);

        window.display();
    }
}

//////////////////////////////////////////////////////////////////
//////////////////////// MAIN DEFINITON //////////////////////////
//////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

    for (int i=0; i<argc; i++) {
        if (i == 0) continue;

        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::cout << "+-------------------- help -------------------+\n";
            std::cout << "| left  click to button - quick save          |\n";
            std::cout << "| right click to button - choose path to save |\n";
            std::cout << "+---------------------------------------------+\n";
            std::cout << "| qalam <file>  -  open this file with qalam  |\n";
            std::cout << "+---------------------------------------------+\n";
            return 0;
        } else if (argv[i][0] != '-') {
            save_filename = argv[i];
        }
    }

    ////////////////////////////////////////////////////
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

    sf::Vector2f windowSize(800.f, 600.f);

    sf::RenderWindow window(
        sf::VideoMode({
            (unsigned int)windowSize.x,
            (unsigned int)windowSize.y
        }),
        "Qalam"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("etc/font.ttf")) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return 1;
    }

    int charsize = 20;
    
    const sf::Glyph& glyph = font.getGlyph('M', charsize, false);
    float charw = glyph.advance;
    
    int cursorY = 0;
    int cursorX = buflen(cursorY);
    

    const float line_height = font.getLineSpacing(charsize);
    const float text_x = 20.f;
    const float text_y = 20.f;


    // std::string& current_line = lines[cursorY];

    sf::Text text(font, lines[cursorY], charsize);
    sf::RectangleShape cursor({2.f, line_height});

    sf::RectangleShape save_button(sf::Vector2f(100, 40));
    save_button.setPosition(sf::Vector2f(windowSize.x-120, windowSize.y-60));
    save_button.setFillColor(sf::Color::Blue);

    sf::Text save_label(font, "Save", 25);
    save_label.setPosition({windowSize.x - 110.f, windowSize.y - 60.f});
    save_label.setFillColor(sf::Color::White);

    cursor.setFillColor(sf::Color::White);
    text.setPosition({20.f, 0.f});
 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                for (int i=0; i<bufc(); i++) {std::cout << lines[i] << '\n';}
                window.close();
            }

            // ========== Left Right keys ==========

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::Left) {
                    if (cursorX > 0) cursorX--;
                    else if (cursorY > 0) {
                        cursorY--;
                        cursorX = buflen(cursorY);
                    }
                }
                if (key->code==sf::Keyboard::Key::Right) {
                    if (cursorX < buflen(cursorY))
                        cursorX++;
                    else if (cursorY != bufc()-1) {
                        cursorX=0; 
                        cursorY++;
                    }
                }

                // ========== Up-Down keys ==========

                if (key->code == sf::Keyboard::Key::Down && cursorY < bufc()-1) {
                    cursorY++;
                    if (cursorX > buflen(cursorY)) {
                        cursorX = buflen(cursorY);
                    }
                }
                if (key->code==sf::Keyboard::Key::Up) {
                    if (cursorY > 0) {
                        cursorY--;
                        if (cursorX > buflen(cursorY)) {
                            cursorX = buflen(cursorY);
                        }
                    }
                }
                
                // ========== Delete keys ==========

                if (key->code == sf::Keyboard::Key::Backspace) {
                    if (cursorX > 0) {
                        lines[cursorY].erase(cursorX - 1, 1);
                        cursorX--;
                    } else if (cursorY > 0) {
                        int old_len = buflen(cursorY - 1);
                        lines[cursorY - 1] += lines[cursorY];
                        lines.erase(lines.begin() + cursorY);
                        cursorY--;
                        cursorX = old_len;
                    }
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < buflen(cursorY)) {
                    lines[cursorY].erase(cursorX, 1);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    for (int i = 0; i < 4; i++) {
                        lines[cursorY].insert(cursorX, 1, ' ');
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
                        make_save_window();
                        // save(lines, save_filename);
                        save_button.setFillColor(sf::Color::Cyan);
                    }
                }
            } else save_button.setFillColor(sf::Color::Blue);

            if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (mouse->wheel == sf::Mouse::Wheel::Vertical) {
                    offsetY -= (int)(mouse->delta * line_height * 3);
                    
                    float max_scroll = std::max(0.f, lines.size() * line_height - windowSize.y + 60.f);
                    if (offsetY < 0) offsetY = 0;
                    if (offsetY > max_scroll) offsetY = max_scroll;
                }
            }
        }
        cursor.setPosition({text_x + cursorX * charw, text_y + cursorY * line_height - offsetY});
        window.clear(sf::Color(30, 30, 30));

        for (int i = 0; i < (int)lines.size(); i++) {
            text.setString(lines[i]);
            text.setPosition({text_x, text_y + i * line_height - offsetY});
            window.draw(text);
        }
        window.draw(cursor);
        window.draw(save_button);
        window.draw(save_label);

        window.display();
    }
}