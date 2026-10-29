#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <string>
#include <vector>
#include <functional>

void save(const std::vector<std::string>& list, std::string filename);

void handle_save_window_event(
    const sf::Event& event, sf::RenderWindow& window,
    std::string& filename,  int& cursorX,
    const std::vector<std::string>& lines
);

void make_save_window(std::string& save_filename, const std::vector<std::string>& lines);

class Button : public sf::Drawable {
private:
    sf::RectangleShape bg;
    sf::Text text;
    sf::Font& font;

    std::function<void()> onClick;
    std::function<void()> onRightClick;
    
    sf::Color normalColor = sf::Color(50, 50, 50);
    sf::Color hoverColor =  sf::Color(80, 80, 80);
    sf::Color activeColor = sf::Color::Cyan;

    int timer = 0;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    Button(sf::Font& f, const std::string& label, sf::Vector2f pos, sf::Vector2f size, 
           std::function<void()> action = nullptr, std::function<void()> action2 = nullptr);

    void handleClick(sf::Vector2f mousePos, bool rightClick);
    void update(sf::Vector2f mousePos);
    void setText(const std::string& newText);
    
    sf::FloatRect getGlobalBounds() const;
    void setColor(const sf::Color& color);
    void setFgColor(const sf::Color& color);
    void setColors(const sf::Color& normal, const sf::Color& click, const sf::Color& hover);
};