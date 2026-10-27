#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

void save(const vector<string>& list, string filename);

void handle_save_window_event(
    const sf::Event& event, sf::RenderWindow& window,
    std::string& filename,  int& cursorX,
    const std::vector<std::string>& lines
);

void make_save_window(std::string& save_filename, const std::vector<std::string>& lines);

class Button : public Drawable {
private:
    RectangleShape bg;
    Text text;
    Font& font;

    function<void()> onClick;
    function<void()> onRightClick;
    
    Color normalColor = Color(50, 50, 50);
    Color hoverColor = Color(80, 80, 80);
    Color activeColor = Color::Cyan;

protected:
    void draw(RenderTarget& target, RenderStates states) const override;

public:
    Button(Font& f, const string& label, Vector2f pos, Vector2f size, 
           function<void()> action = nullptr, function<void()> action2 = nullptr);

    void handleClick(Vector2f mousePos, bool rightClick);
    void update(Vector2f mousePos);
    void setText(const string& newText);
    
    FloatRect getGlobalBounds() const;
    void setColor(const Color& color);
    void setColors(const Color& normal, const Color& click, const Color& hover);
};