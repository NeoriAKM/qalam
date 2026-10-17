#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "inc/saving.hpp"

void save(const std::vector<std::string>& list, std::string filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {std::cerr << "Cannot open file for writing\n"; return;}
    for (const auto& line : list) out << line << '\n';
}

void handle_save_window_event(
    const sf::Event& event,
    sf::RenderWindow& window,
    std::string& filename,
    int& cursorX,
    const std::vector<std::string>& lines
    ) {
    
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Left && cursorX > 0) cursorX--;
        if (key->code == sf::Keyboard::Key::Right && cursorX < (int)filename.size()) cursorX++;

        if (key->code == sf::Keyboard::Key::Down) cursorX = 0;
        if (key->code == sf::Keyboard::Key::Up)   cursorX = (int)filename.size();

        if (key->code == sf::Keyboard::Key::Backspace && cursorX > 0) {
            filename.erase(cursorX - 1, 1);
            cursorX--;
        }
        if (key->code == sf::Keyboard::Key::Delete && cursorX < (int)filename.size()) {
            filename.erase(cursorX, 1);
        }
        if (key->code == sf::Keyboard::Key::Enter) {
            save(lines, filename);
            window.close();
        }
    }

    if (const auto* t = event.getIf<sf::Event::TextEntered>()) {
        if (t->unicode >= 32 && t->unicode < 127) {
            filename.insert(cursorX, 1, static_cast<char>(t->unicode));
            cursorX++;
        }
    }
}

////////////////////////////////////////////////////////////

void make_save_window(std::string save_filename, const std::vector<std::string>& lines) {
    sf::Vector2f windowSize(500.f, 100.f);

    sf::RenderWindow window(sf::VideoMode(
        {(unsigned int)windowSize.x, (unsigned int)windowSize.y}
    ), "Save window");
    window.setFramerateLimit(30);

    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSansMono.ttf")) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return;
    }

    int charsize = 15;
    
    const sf::Glyph& glyph = font.getGlyph('M', charsize, false);
    float charw = glyph.advance;
    
    int cursorX = (int)save_filename.size();

    const float line_height = font.getLineSpacing(charsize);

    sf::Text text(font, save_filename, charsize);
    sf::RectangleShape cursor({2.f, line_height});

    cursor.setFillColor(sf::Color::White);
    text.setPosition({10.f, 5.f});
 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            handle_save_window_event(*event, window, save_filename, cursorX, lines);
        }
        text.setString(save_filename);
        cursor.setPosition({10.f + cursorX * charw, 5.f});
        window.clear(sf::Color(30, 30, 30));

        window.draw(text);
        window.draw(cursor);

        window.display();
    }
}