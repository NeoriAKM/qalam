#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <fstream>
#include "inc/dialogues.hpp"
#include "inc/config.hpp"

using namespace std;
using namespace sf;

void save(const vector<string>& list, string filename) {
    ofstream out(filename);
    if (!out.is_open()) {cerr << "Cannot open file for writing\n"; return;}
    for (const auto& line : list) out << line << '\n';
}

void handle_save_window_event(
    const Event& event, RenderWindow& window,
    string& buffer, int& cursorX,
    const std::vector<string>& lines
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == Keyboard::Key::Left && cursorX > 0) cursorX--;
        if (key->code == Keyboard::Key::Right && cursorX < (int)buffer.size()) cursorX++;

        if (key->code == Keyboard::Key::Backspace && cursorX > 0) {
            buffer.erase(cursorX - 1, 1);
            cursorX--;
        }
        if (key->code == Keyboard::Key::Delete && cursorX < (int)buffer.size()) {
            buffer.erase(cursorX, 1);
        }
        if (key->code == Keyboard::Key::Enter) {
            save(lines, buffer);
            window.close();
        }
        if (key->code == Keyboard::Key::Escape) window.close();
    }

    if (const auto* t = event.getIf<sf::Event::TextEntered>()) {
        if (t->unicode >= 32 && t->unicode < 127) {
            buffer.insert(cursorX, 1, static_cast<char>(t->unicode));
            cursorX++;
        }
    }
}

////////////////////////////////////////////////////////////

void make_save_window(string& save_filename, const vector<string>& lines) {
    Config config;
    config.load();

    string buffer = save_filename;
    
    Vector2f windowSize(500.f, 100.f);

    RenderWindow window(VideoMode(
        {(unsigned int)windowSize.x, (unsigned int)windowSize.y}
    ), "Save window");
    window.setFramerateLimit(30);

    Font font;
    if (!font.openFromFile(config.font_path)) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return;
    }

    int charsize = 15;
    
    const Glyph& glyph = font.getGlyph('M', charsize, false);
    float charw = glyph.advance;
    
    int cursorX = (int)buffer.size();

    const float line_height = font.getLineSpacing(charsize);

    Text text(font, buffer, charsize);
    RectangleShape cursor({2.f, line_height});

    cursor.setFillColor(Color::White);
    text.setPosition({10.f, 5.f});
 
    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            handle_save_window_event(*event, window, buffer, cursorX, lines);
        }
        text.setString(buffer);
        cursor.setPosition({10.f + cursorX * charw, 5.f});
        window.clear(sf::Color(30, 30, 30));

        window.draw(text);
        window.draw(cursor);

        window.display();
    }
}

Button::Button(Font& f, const string& label, Vector2f pos,
               Vector2f size, function<void()> action, function<void()> action2) 
               : font(f), text(f), onClick(action), onRightClick(action2)
    {
    
    bg.setPosition(pos);
    bg.setSize(size);
    bg.setFillColor(Color(50, 50, 50));
    bg.setOutlineThickness(2);
    bg.setOutlineColor(Color::White);
   
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(20);
    text.setFillColor(Color::White);
   
    FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
    text.setPosition({pos.x + size.x / 2.0f, pos.y + size.y / 2.0f});
}

sf::FloatRect Button::getGlobalBounds() const {return bg.getGlobalBounds();}

void Button::setColor(const sf::Color& color) {bg.setFillColor(color);}
void Button::setFgColor(const sf::Color& color) {text.setFillColor(color);}

void Button::setText(const string& newText) {
    text.setString(newText);
    FloatRect textBounds = text.getLocalBounds();

    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f, 
        textBounds.position.y + textBounds.size.y / 2.0f
    });

    text.setPosition({
        bg.getPosition().x + bg.getSize().x / 2.0f,
        bg.getPosition().y + bg.getSize().y / 2.0f
    });
}

void Button::draw(RenderTarget& target, RenderStates states) const {
    target.draw(bg, states);
    target.draw(text, states);
}

void Button::update(sf::Vector2f mousePos) {
    if (timer > 0) {
        bg.setFillColor(activeColor);
    } else if (bg.getGlobalBounds().contains(mousePos)) {
        bg.setFillColor(hoverColor);
    } else {
        bg.setFillColor(normalColor);
    }
    timer = std::max(timer-1, 0);
}

void Button::handleClick(sf::Vector2f mousePos, bool rightClick) {
    if (!bg.getGlobalBounds().contains(mousePos)) return;
    
    if (rightClick) {
        if (onRightClick) onRightClick(); 
    } else {
        if (onClick) onClick();
    }

    timer = 20;
    
    bg.setFillColor(activeColor);
}

void Button::setColors(const Color& normal, const Color& click, const Color& hover) {
    normalColor = normal;
    hoverColor  = hover;
    activeColor = click;
}