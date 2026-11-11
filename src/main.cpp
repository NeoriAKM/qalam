// ---------------------------- //
// Qalam - Text editor v1.3.3   //
// Copyright NeoriAKM(c)        //
// ---------------------------- //

#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shape.hpp>

extern "C" {
    #include <wren.h>
}

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <optional>

#include "inc/dialogues.hpp"
#include "inc/config.hpp"
#include "inc/syntax_hl.hpp"
#include "inc/helper.hpp"

/*
std::vector<std::string> fonts = {
    "etc/font.ttf",                                        // classic
    "/usr/share/fonts/TTF/DejaVuSansMono.ttf",             // Arch
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", // Debian
    };
    ! Coming soon
*/

using namespace std;

void writeFn(WrenVM* /* vm */, const char* text) {cout << text;}

void errorFn(
    WrenVM* /* vm */, WrenErrorType errorType,
    const char* module, const int line, const char* msg)
    {

    if (errorType == WREN_ERROR_COMPILE) {
        std::cerr << "[WREN COMPILE ERROR] " << module << ":" << line << " - " << msg << "\n";
    } else if (errorType == WREN_ERROR_RUNTIME) {
        std::cerr << "[WREN RUNTIME ERROR] " << msg << "\n";
    } else if (errorType == WREN_ERROR_STACK_TRACE) {
        std::cerr << "[WREN TRACE] " << module << ":" << line << " - " << msg << "\n";
    }
}

//////////////////////////////////////////////////////////////////
//////////////////////// MAIN DEFINITON //////////////////////////
//////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    Flags flag;
    Args args = parse_args(argc, argv);

    vector<string> lines = {};
    string save_filename = args.filename;
    
    sf::Color theme_color1 = sf::Color::White; // fg
    sf::Color theme_color2 = sf::Color::Black; // bg
    
    /////////////// config //////////////////

    Config config;
    config.load();
    
    string font_path = config.font_path;
    unsigned int font_size = config.font_size;
    string theme = config.theme;
    unsigned int fps = config.fps;

    int w_width = max(200, config.w_width);
    int w_height= max(200, config.w_height);
    
    string placeholder = config.placeholder;
    string unnamed = config.unnamed;
    
    if (flag.debug) {
        vector<pair<string,string>> cfg_base = {
            {"Font",        font_path},
            {"Size",        to_string(font_size)},
            {"Theme",       theme},
            {"Window",      to_string(w_width) + "x" + to_string(w_height)},
            {"Placeholder", placeholder},
            {"Unnamed",     unnamed},
            {"FPS",         to_string(fps)},
        };
        
        
        // output
        cout << repeat("-", 7) << " Config values " << repeat("-",8);
        for (auto& [a, b] : cfg_base) cout <<a<<": "<<b<<'\n';
        cout << repeat("-", 30);
    }
    
    
    if (theme == "light") {
        theme_color1 = sf::Color(50, 50, 50);
        theme_color2 = sf::Color::White;
    }
    
    ///////////////////////////////////////////////
    
    WrenConfiguration wrencfg;
    wrenInitConfiguration(&wrencfg);

    wrencfg.writeFn = writeFn;
    wrencfg.errorFn = errorFn;

    // getting information from mod
    ifstream mod_file("/home/akm/.config/qalam/mod1.wren");
    stringstream mod_buffer;
    mod_buffer << mod_file.rdbuf();
    string mod_src = mod_buffer.str();

    WrenVM* vm = wrenNewVM(&wrencfg);
    if (!vm) {cerr << "Failed to create WrenVM!\n"; return 1;}
    
    WrenHandle* update_handle = nullptr;
    
    if (flag.mods) {
        WrenInterpretResult res = wrenInterpret(vm, "main", mod_src.c_str());
        if (res != WREN_RESULT_SUCCESS) cerr << "[WREN] Failed to load mod!\n";
        else {
            cout << "[WREN] Mod is loaded!\n";

            wrenEnsureSlots(vm, 1);
            wrenGetVariable(vm, "main", "update", 0);
            if (wrenGetSlotType(vm,0)!=WREN_TYPE_NULL) update_handle = wrenGetSlotHandle(vm, 0);

            else cout << "[WREN] No update() function found in mod.\n";
        }
    }
    

    //////////////////////////////////////////////////////

    if (save_filename == "") save_filename = unnamed;
    
    ifstream in(save_filename);
    
    if (in.is_open()) {
        string line;
        while (getline(in, line)) lines.push_back(line);
        in.close();
    }
    if (lines.empty()) lines.push_back(placeholder);

    sf::Vector2f windowSize(w_width, w_height);

    sf::RenderWindow window(
        sf::VideoMode({(unsigned int)windowSize.x, (unsigned int)windowSize.y}),
        sf::String("Qalam: '") + sf::String(save_filename) + "'"
    );

    window.setFramerateLimit(fps);

    sf::Font font;
    if (!font.openFromFile(font_path)) {
        std::cout << "Font not found, Sorry. [return 1]\n";
        return 1;
    }
    
    const sf::Glyph& glyph1 = font.getGlyph('M', font_size, false);
    const sf::Glyph& glyph2 = font.getGlyph('i', font_size, false);

    if (glyph1.advance != glyph2.advance) {
        std::cerr << "\nWARNING: Please, can you use mono font? '" <<
                    font_path << "' is not mono, or not fully mono font\n";
    }

    float charw = glyph1.advance;

    float     gaps = charw*(to_string(lines.size()).size()+1);
    float old_gaps = 1.f;
    
    size_t cursorY = lines.size()-1;
    size_t cursorX = buflen(cursorY, lines);

    unsigned int tick = 0;                     
    
    float offsetY = 0;    float offsetX = 0;
    size_t first_visible; size_t last_visible;

    std::string cb_text = "";

    const float line_height = font.getLineSpacing(font_size);
    const float text_x = charw; const float text_y = charw;

    sf::Text    text(font, lines[cursorY], font_size);
    sf::Text strnums(font, "",             font_size);
    sf::RectangleShape cursor({2.f, line_height});
    sf::RectangleShape numsbg({gaps + (charw*2), windowSize.y}); 


    text.setPosition({charw, 0.f});
    strnums.setPosition({charw, 0.f});
    numsbg.setPosition({0.f, 0.f});
    
    text.setFillColor(theme_color1);
    strnums.setFillColor(theme_color1);
    cursor.setFillColor(theme_color1);
    numsbg.setFillColor(sf::Color(40, 40, 40, 220));

    Button save_button(font, "save",
        {windowSize.x-120, windowSize.y-60}, {100, 40},
        [&]() {save(lines, save_filename);},
        [&]() {make_save_window(save_filename, lines);}
    );
    Button cursor_button(font, "???",
        {20, windowSize.y-60}, {200, 40}, [&]() {
        cursorY = (last_visible + first_visible) / 2;
        cursorX = buflen(cursorY, lines);
    });
    Button strings_button(
        font, std::to_string(lines.size()),
        {240, windowSize.y-60}, {100, 40}, [&]() {}
    );
    save_button.setColors(
        sf::Color(40, 160, 40, 200),
        sf::Color(70, 255, 70, 200),
        sf::Color(50, 210, 50, 200)
    );
    cursor_button.setColors(
        sf::Color(60, 60, 60, 200),
        sf::Color(100, 100, 100, 200),
        sf::Color(80, 80, 80, 200)
    );
    strings_button.setColors(
        sf::Color(175, 175, 100, 200),
        sf::Color(255, 255, 150, 200),
        sf::Color(210, 210, 120, 200)
    );

    /* ! //////////////////// MAINLOOP ///////////////////////// ! */

    while (window.isOpen()) {
        if (flag.mods){
            if (update_handle != nullptr) {
                WrenInterpretResult result = wrenCall(vm, update_handle);
                if (result != WREN_RESULT_SUCCESS) {
                    std::cerr << "[WREN ERROR] Running caught error\n";
                }
            }
        }

        /////////////////////////////////////////////////////////

        tick++;

        sf::Vector2f mousePos(
            (float)sf::Mouse::getPosition(window).x, 
            (float)sf::Mouse::getPosition(window).y
        );

        cb_text = "x: " + std::to_string(cursorX) + " | y: " + std::to_string(cursorY);

        save_button.update(mousePos);
        cursor_button.update(mousePos);
        strings_button.update(mousePos);
        
        if (old_gaps != gaps) numsbg.setSize({gaps + (charw/2), windowSize.y});

        cursor_button.setText(cb_text);
        strings_button.setText(std::to_string((int)lines.size()));
        
        first_visible = static_cast<size_t>(max(0.f, offsetY / line_height));
        last_visible  = min(
            lines.size() - 1,
            static_cast<size_t>(max(0.f, (offsetY + windowSize.y) / line_height)) + 1
        );

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

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
                    else if (cursorY != lines.size()-1) {
                        cursorX=0; 
                        cursorY++;
                    }
                }

                // ========== Up-Down keys ==========
                float max_scroll = std::max(0.f, lines.size() * line_height - windowSize.y + 100.f);

                if (key->code == sf::Keyboard::Key::Down && cursorY < lines.size()-1) {
                    cursorY++;

                    if (cursorY > last_visible)  offsetY += line_height;

                    if (offsetY > max_scroll) offsetY = max_scroll;
                    if (cursorX > buflen(cursorY, lines)) {
                        cursorX = buflen(cursorY, lines);
                    }
                }
                if (key->code==sf::Keyboard::Key::Up) {
                    if (cursorY > 0) {
                        cursorY--;

                        if (cursorY < first_visible) offsetY -= line_height;

                        if (offsetY < 0) offsetY = 0;
                        if (cursorX > buflen(cursorY, lines)) {
                            cursorX = buflen(cursorY, lines);
                        }
                    }
                }
                
                // ========== Delete keys ==========

                if (key->code == sf::Keyboard::Key::Backspace) {
                    if (key->control) {
                        while (cursorX > 0 && lines[cursorY][cursorX - 1] == ' ') cursorX--;
                        while (cursorX > 0 && lines[cursorY][cursorX - 1] != ' ') {
                            lines[cursorY].erase(cursorX - 1, 1);
                            cursorX--;
                        }
                        continue;
                    }
                    // simple backspace
                    if (lines[cursorY] == placeholder) {lines[cursorY] = ""; cursorX = 0;}
                    if (cursorX > 0) {
                        lines[cursorY].erase(cursorX - 1, 1);
                        cursorX--;
                    } else if (cursorY > 0) {
                        size_t old_len = buflen(cursorY - 1, lines);
                        lines[cursorY - 1] += lines[cursorY];
                        lines.erase(lines.begin() + static_cast<ptrdiff_t>(cursorY));
                        cursorY--;
                        cursorX = old_len;

                        if (cursorY >= lines.size()) cursorY = lines.size() - 1;
                        if (cursorX > buflen(cursorY, lines)) cursorX = buflen(cursorY, lines);

                        if (offsetY > max_scroll) offsetY = (int)max_scroll;
                        if (offsetY < 0) offsetY = 0;

                        first_visible = static_cast<size_t>(max(0, (int)(offsetY / line_height)));
                        last_visible  = min(
                            lines.size() - 1,
                            static_cast<size_t>(max(0.f, (offsetY + windowSize.y) / line_height)) + 1
                        );
                    }
                }
                if (key->code == sf::Keyboard::Key::Delete && cursorX < buflen(cursorY, lines)) {
                    lines[cursorY].erase(cursorX, 1);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    int spaces = 4 - (cursorX % 4);
                    for (int i = 0; i < spaces; i++) {
                        lines[cursorY].insert(cursorX, 1, ' ');
                        cursorX++;
                    }
                }
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::string rest = lines[cursorY].substr(cursorX);
                    lines[cursorY].erase(cursorX);

                    size_t indent = 0;
                    while (indent < lines[cursorY].size() && lines[cursorY][indent] == ' ') indent++;

                    lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(cursorY+1), (std::string(indent, ' ') + rest));
                    cursorY++;
                    cursorX = indent;
                }
                /////////// Ctrl+Something ///////////
                // ! CURSOR ACTS
                if (key->code == sf::Keyboard::Key::U && key->control && !key->shift) { // ctrl+U
                    cursorY = (last_visible + first_visible) / 2;
                    cursorX = buflen(cursorY, lines);
                }

                // ! SAVES
                if (key->code == sf::Keyboard::Key::S && key->control && !key->shift) { // ctrl+S
                    save(lines, save_filename);
                }
                if (key->code == sf::Keyboard::Key::S && key->control && key->shift) {  // Ctrl+Shift+S
                    make_save_window(save_filename, lines);
                }

                // ! COPIES
                if (key->code == sf::Keyboard::Key::C && key->control && !key->shift) {  // Ctrl+C
                    copy_string(lines, cursorY);
                }
                if (key->code == sf::Keyboard::Key::C && key->control && key->shift)  { // Ctrl+Shift+C
                    copy_vec(lines);
                }
                if (key->code == sf::Keyboard::Key::V && key->control && !key->shift) { // Ctrl+V
                    const string clipboard = sf::Clipboard::getString().toAnsiString();
                    lines[cursorY].insert(cursorX, clipboard);
                    cursorX += clipboard.size();
                }
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    char typed = static_cast<char>(t->unicode);

                    if      (typed == '(') {lines[cursorY].insert(cursorX, "()");}
                    else if (typed == '[') {lines[cursorY].insert(cursorX, "[]");}
                    else if (typed == '{') {lines[cursorY].insert(cursorX, "{}");}
                    else if (typed == '"') {lines[cursorY].insert(cursorX, "\"\"");}
                    else                   {lines[cursorY].insert(cursorX, 1, typed);}
                    cursorX++;
                }
            }

            //////////////// Buttons ////////////////

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f pos((float)mouse->position.x, (float)mouse->position.y);

                if (mouse->button == sf::Mouse::Button::Left) {
                    save_button.handleClick(pos, false);
                    cursor_button.handleClick(pos, false);
                } else if (mouse->button == sf::Mouse::Button::Right) {
                    save_button.handleClick(pos, true);
                    cursor_button.handleClick(pos, true);
                }
            }

            /* ? MOUSE SCROLL PART ¿ */

            if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>()) {
    
                bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
                
                bool ctrl_pressed  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                
                int mod = 1;
                if (ctrl_pressed) mod = 2;

                if (mouse->wheel == sf::Mouse::Wheel::Vertical) {
                    if (shift_pressed) {
                        offsetX -= (int)(mouse->delta * charw * 5 * mod);

                        int max_line_length = 0;
                        for(const auto&line:lines) {
                            if((int)line.size()>max_line_length)max_line_length=line.size();
                        }
                        float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                        if (offsetX < 0) offsetX = 0;
                        if (offsetX > max_scroll) offsetX = max_scroll;

                    } else {
                        // Vertical
                        offsetY -= (int)(mouse->delta * line_height * 3 * mod);
                        float max_scroll = std::max(0.f, lines.size() * line_height - windowSize.y + 100.f);
                        if (offsetY < 0) offsetY = 0;
                        if (offsetY > max_scroll) offsetY = max_scroll;
                    }
                } 
                // Horizontal touchpad
                else if (mouse->wheel == sf::Mouse::Wheel::Horizontal) {
                    offsetX -= (int)(mouse->delta * charw * 5 * mod);

                    int max_line_length = 0;
                    for(const auto&line:lines)if((int)line.size()>max_line_length)max_line_length=line.size();
                    float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                    if (offsetX < 0) offsetX = 0;
                    if (offsetX > max_scroll) offsetX = max_scroll;
                }
            }
            // if (cursorY > last_visible) offsetY++;
            if (cursorX * charw > windowSize.x + offsetX) offsetX += charw;
        }

        gaps = charw * (to_string(lines.size()).size()+1);

        cursor.setPosition({
            text_x + cursorX * charw + gaps - offsetX,
            text_y + cursorY * line_height - offsetY
        });

        if (theme == "dark") window.clear(sf::Color(30, 30, 30));
        else window.clear(sf::Color(255, 239, 223));

        for (size_t i = first_visible; i <= last_visible; i++) {
            text.setFillColor(HL(lines[i], save_filename, theme_color1));
            text.setString(lines[i]);
            text.setPosition({text_x + gaps - offsetX, text_y + i * line_height - offsetY});
            
            window.draw(text);
        }
        
        window.draw(numsbg);

        for (size_t i = first_visible; i <= last_visible; i++) {
            strnums.setString(to_string(i+1));
            sf::FloatRect bounds = strnums.getLocalBounds();
            strnums.setOrigin({bounds.position.x + bounds.size.x + (charw * 1.5f), 0.f});
            strnums.setPosition({text_x + gaps, text_y + i * line_height - offsetY});

            window.draw(strnums);
        }
        
        if (tick < fps/2) window.draw(cursor);
        else if (tick > fps-1) tick = 0;        

        window.draw(save_button);
        window.draw(cursor_button);
        window.draw(strings_button);

        window.display();

        old_gaps = gaps;
    }
    if (update_handle != nullptr) wrenReleaseHandle(vm, update_handle);
    wrenFreeVM(vm);
    return 0;
}