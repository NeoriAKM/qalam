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

/*-------------------------------------------------------*/

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
                for (int i=0; i<bufc(); i++) {std::cout << lines[i];}
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::Left  && cursorX > 0) cursorX--;
                if (key->code==sf::Keyboard::Key::Right && cursorX < buflen(cursorY)) cursorX++;

                if (key->code==sf::Keyboard::Key::Down && cursorY < bufc()-1) cursorY++;
                if (key->code==sf::Keyboard::Key::Up   && cursorY > 0) cursorY--;

                if (key->code == sf::Keyboard::Key::Backspace && cursorX > 0) {
                    lines[cursorY].erase(cursorX-1, 1);
                    cursorX--;
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