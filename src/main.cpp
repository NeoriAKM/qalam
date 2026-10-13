// Qalam - Text editor
// Copyright NeoriAKM(c)

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
// #include <fstream>

// std::ifstream textfile("etc/text.txt");
std::vector<std::string> lines;
std::string placeholder = "Qalam Text Editor. Live long and prosper!";

int buflen(int line) {return (int)lines[line].size();}
int bufc() {return (int)lines.size();}

/*-------------------------------------------------------*/

int main() {
    /*
    if (!textfile.is_open()) {
        std::cerr << "WARN: File unaviable to open\n";
        return 1;
    }
    */

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Qalam");
    sf::Font font;
    if (!font.openFromFile("etc/font.ttf")) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return 1;
    }
    lines.emplace_back(placeholder);

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
                        lines[cursorY - 1] += lines[cursorY];
                        lines.erase(lines.begin() + cursorY);
                        cursorY--;
                        cursorX = buflen(cursorY - 1);
                    }
                    text.setString(lines[cursorY]);
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < buflen(cursorY)) {
                    lines[cursorY].erase(cursorX, 1);
                    text.setString(lines[cursorY]);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    for (int i = 0; i < 4; i++) {
                        lines[cursorY].insert(cursorX, 1, ' ');
                        cursorX++;
                    }
                    text.setString(lines[cursorY]);
                }
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::string rest = lines[cursorY].substr(cursorX);
                    lines[cursorY].erase(cursorX);
                    lines.insert(lines.begin() + cursorY + 1, rest);
                    cursorY++;
                    cursorX=0;
                    text.setString(lines[cursorY]);
                }
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    lines[cursorY].insert(cursorX, 1, static_cast<char>(t->unicode));
                    cursorX++;
                    text.setString(lines[cursorY]);
                }
            }
        }

        cursor.setPosition({text_x + cursorX * charw, text_y + cursorY * line_height});
        window.clear(sf::Color(30, 30, 30));

        for (int i = 0; i < (int)lines.size(); i++) {
            text.setString(lines[i]);
            text.setPosition({text_x, text_y + i * line_height});
            window.draw(text);
        }
        window.draw(cursor);

        window.display();
    }
}