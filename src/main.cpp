// Qalam - Text editor
// Copyright NeoriAKM(c)

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

std::vector<std::string> lines = {
    "Qalam Text Editor. Live long and prosper!"
};

int buflen(int line) {return (int)lines[line].size();}
int bufc() {return (int)lines.size();}

int main() {

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Qalam");
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return 1;
    }

    int charsize = 20;
    
    const sf::Glyph& glyph = font.getGlyph('M', charsize, false);
    float charw = glyph.advance;
    
    int cursorY = 0;
    int cursorX = buflen(cursorY);

    std::string& current_line = lines[cursorY];


    sf::Text text(font, current_line, charsize);
    sf::RectangleShape cursor({2.f, 20.f});

    cursor.setFillColor(sf::Color::White);
    text.setPosition({20.f, 0.f});
 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::Left  && cursorX > 0) cursorX--;
                if (key->code==sf::Keyboard::Key::Right && cursorX < buflen(cursorY)) cursorX++;

                if (key->code==sf::Keyboard::Key::Down) cursorX = buflen(cursorY);
                if (key->code==sf::Keyboard::Key::Up  ) cursorX = 0;

                if (key->code == sf::Keyboard::Key::Backspace && cursorX > 0) {
                    current_line.erase(cursorX-1, 1);
                    cursorX--;
                    text.setString(current_line);
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < buflen(cursorY)) {
                    current_line.erase(cursorX, 1);
                    text.setString(current_line);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    for (int i = 0; i < 4; i++) {
                        current_line.insert(cursorX, 1, ' ');
                        cursorX++;
                    }
                    text.setString(current_line);
                }
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    current_line.insert(cursorX, 1, static_cast<char>(t->unicode));
                    cursorX++;
                    text.setString(current_line);
                }
            }
        }

        cursor.setPosition({20.f + cursorX * charw, 0.f});

        window.clear(sf::Color(30, 30, 30));

        window.draw(text);
        window.draw(cursor);

        window.display();
    }
}