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
// #include <optional>

#include "inc/dialogues.hpp"
#include "inc/config.hpp"
#include "inc/syntax_hl.hpp"
#include "inc/helper.hpp"
#include "inc/api.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////
//////////////////////// MAIN DEFINITON //////////////////////////
//////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    Flags flag;
    Args args = parse_args(argc, argv);
    Editor editor;
    
    sf::Color theme_color1 = sf::Color::White; // fg
    sf::Color theme_color2 = sf::Color::Black; // bg
    
    /////////////// config //////////////////

    Config config;
    config.load();
    
    editor.save_filename = args.filename;
    
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

    WrenVM* vm = nullptr;
    WrenHandle* update_handle = nullptr;

    // getting information from mod
    if (flag.mods) {
        ifstream mod_file(string(getenv("HOME")) + "/.config/qalam/mod1.wren");
        if (!mod_file.is_open()) {
            cerr << "[WREN] Mod file not found. Skipping mods.\n";
        } else {
            stringstream mod_buffer;
            mod_buffer << mod_file.rdbuf();
            string mod_src = mod_buffer.str();
                
            vm = wrenNewVM(&wrencfg);
            if (!vm) {cerr << "Failed to create WrenVM!\n"; return 1;}
        
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
    }
    

    //////////////////////////////////////////////////////

    if (editor.save_filename == "") editor.save_filename = unnamed;
    
    ifstream in(editor.save_filename);
    
    if (in.is_open()) {
        string line;
        while (getline(in, line)) editor.lines.push_back(line);
        in.close();
    }
    if (editor.lines.empty()) editor.lines.push_back(placeholder);

    sf::Vector2f windowSize(w_width, w_height);

    sf::RenderWindow window(
        sf::VideoMode({(unsigned int)windowSize.x, (unsigned int)windowSize.y}),
        sf::String("Qalam: '") + sf::String(editor.save_filename) + "'"
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

    float     gaps = charw*(to_string(editor.lines.size()).size()+1);
    float old_gaps = 1.f;

    unsigned int tick = 0;                     

    size_t first_visible; size_t last_visible;

    std::string cb_text = "";

    const float line_height = font.getLineSpacing(font_size);
    const float text_x = charw; const float text_y = charw;

    sf::Text    text(font, editor.lines[editor.cursorY], font_size);
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

    editor.buttons = {
        Button(font, "save",  // ! save button
            {windowSize.x-120, windowSize.y-60}, {100, 40},
            [&]() {save(editor.lines, editor.save_filename);},
            [&]() {make_save_window(editor.save_filename, editor.lines);}
        ),
        Button(font, "???",   // ! cursor pos button
            {20, windowSize.y-60}, {200, 40}, [&]() {
            editor.cursorY = (last_visible + first_visible) / 2;
            editor.cursorX = buflen(editor.cursorY, editor.lines);
        }),
        Button(                        // ! string button
            font, to_string(editor.lines.size()),
            {240, windowSize.y-60}, {100, 40}, [&]() {}
        )
    };

    editor.buttons[0].setColors(
        sf::Color(40, 160, 40, 200),
        sf::Color(70, 255, 70, 200),
        sf::Color(50, 210, 50, 200)
    );
    editor.buttons[1].setColors(
        sf::Color(60, 60, 60, 200),
        sf::Color(100, 100, 100, 200),
        sf::Color(80, 80, 80, 200)
    );
    editor.buttons[2].setColors(
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

        cb_text = "x: " + to_string(editor.cursorX) + " | y: " + to_string(editor.cursorY);

        for (auto& b : editor.buttons) b.update(mousePos); // btns updatinh
        
        if (old_gaps != gaps) numsbg.setSize({gaps + (charw/2), windowSize.y});

        editor.buttons[1].setText(cb_text);
        editor.buttons[2].setText(to_string((int)editor.lines.size()));
        
        first_visible = static_cast<size_t>(max(0.f, editor.offsetY / line_height));
        last_visible  = min(
            editor.lines.size() - 1,
            static_cast<size_t>(max(0.f, (editor.offsetY + windowSize.y) / line_height)) + 1
        );

        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // ========== Left Right keys ==========

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::Left) {
                    if (editor.cursorX > 0) editor.cursorX--;
                    else if (editor.cursorY > 0) {
                        editor.cursorY--;
                        editor.cursorX = buflen(editor.cursorY, editor.lines);
                    }
                }
                if (key->code==sf::Keyboard::Key::Right) {
                    if (editor.cursorX < buflen(editor.cursorY, editor.lines))
                        editor.cursorX++;
                    else if (editor.cursorY != editor.lines.size()-1) {
                        editor.cursorX=0; 
                        editor.cursorY++;
                    }
                }

                // ========== Up-Down keys ==========
                float max_scroll = std::max(0.f, editor.lines.size() * line_height - windowSize.y + 100.f);

                if (key->code == sf::Keyboard::Key::Down && editor.cursorY < editor.lines.size()-1) {
                    editor.cursorY++;

                    if (editor.cursorY > last_visible)  editor.offsetY += line_height;

                    if (editor.offsetY > max_scroll) editor.offsetY = max_scroll;
                    if (editor.cursorX > buflen(editor.cursorY, editor.lines)) {
                        editor.cursorX = buflen(editor.cursorY, editor.lines);
                    }
                }
                if (key->code==sf::Keyboard::Key::Up) {
                    if (editor.cursorY > 0) {
                        editor.cursorY--;

                        if (editor.cursorY < first_visible) editor.offsetY -= line_height;

                        if (editor.offsetY < 0) editor.offsetY = 0;
                        if (editor.cursorX > buflen(editor.cursorY, editor.lines)) {
                            editor.cursorX = buflen(editor.cursorY, editor.lines);
                        }
                    }
                }
                
                // ========== Delete keys ==========

                if (key->code == sf::Keyboard::Key::Backspace) {
                    if (key->control) {
                        while (editor.cursorX > 0 && editor.lines[editor.cursorY][editor.cursorX - 1] == ' ') editor.cursorX--;
                        while (editor.cursorX > 0 && editor.lines[editor.cursorY][editor.cursorX - 1] != ' ') {
                            editor.lines[editor.cursorY].erase(editor.cursorX - 1, 1);
                            editor.cursorX--;
                        }
                        continue;
                    }
                    // simple backspace
                    if (editor.lines[editor.cursorY] == placeholder) {editor.lines[editor.cursorY] = ""; editor.cursorX = 0;}
                    if (editor.cursorX > 0) {
                        editor.lines[editor.cursorY].erase(editor.cursorX - 1, 1);
                        editor.cursorX--;
                    } else if (editor.cursorY > 0) {
                        size_t old_len = buflen(editor.cursorY - 1, editor.lines);
                        editor.lines[editor.cursorY - 1] += editor.lines[editor.cursorY];
                        editor.lines.erase(editor.lines.begin() + static_cast<ptrdiff_t>(editor.cursorY));
                        editor.cursorY--;
                        editor.cursorX = old_len;

                        if (editor.cursorY >= editor.lines.size()) editor.cursorY = editor.lines.size() - 1;
                        if (editor.cursorX > buflen(editor.cursorY, editor.lines))
                            editor.cursorX = buflen(editor.cursorY, editor.lines);

                        if (editor.offsetY > max_scroll) editor.offsetY = (int)max_scroll;
                        if (editor.offsetY < 0) editor.offsetY = 0;

                        first_visible = static_cast<size_t>(max(0, (int)(editor.offsetY / line_height)));
                        last_visible  = min(
                            editor.lines.size() - 1,
                            static_cast<size_t>(max(0.f, (editor.offsetY + windowSize.y) / line_height)) + 1
                        );
                    }
                }
                if (key->code == sf::Keyboard::Key::Delete && editor.cursorX < buflen(editor.cursorY, editor.lines)) {
                    editor.lines[editor.cursorY].erase(editor.cursorX, 1);
                }

                if (key->code == sf::Keyboard::Key::Tab) {
                    int spaces = 4 - (editor.cursorX % 4);
                    for (int i = 0; i < spaces; i++) {
                        editor.lines[editor.cursorY].insert(editor.cursorX, 1, ' ');
                        editor.cursorX++;
                    }
                }
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::string rest = editor.lines[editor.cursorY].substr(editor.cursorX);
                    editor.lines[editor.cursorY].erase(editor.cursorX);

                    size_t indent = 0;
                    while (indent < editor.lines[editor.cursorY].size() && editor.lines[editor.cursorY][indent] == ' ') indent++;

                    editor.lines.insert(editor.lines.begin() + static_cast<std::ptrdiff_t>(editor.cursorY+1), (std::string(indent, ' ') + rest));
                    editor.cursorY++;
                    editor.cursorX = indent;
                }
                /////////// Ctrl+Something ///////////
                // ! CURSOR ACTS
                if (key->code == sf::Keyboard::Key::U && key->control && !key->shift) { // ctrl+U
                    editor.cursorY = (last_visible + first_visible) / 2;
                    editor.cursorX = buflen(editor.cursorY, editor.lines);
                }

                // ! SAVES
                if (key->code == sf::Keyboard::Key::S && key->control && !key->shift) { // ctrl+S
                    save(editor.lines, editor.save_filename);
                }
                if (key->code == sf::Keyboard::Key::S && key->control && key->shift) {  // Ctrl+Shift+S
                    make_save_window(editor.save_filename, editor.lines);
                }

                // ! COPIES
                if (key->code == sf::Keyboard::Key::C && key->control && !key->shift) {  // Ctrl+C
                    copy_string(editor.lines, editor.cursorY);
                }
                if (key->code == sf::Keyboard::Key::C && key->control && key->shift)  { // Ctrl+Shift+C
                    copy_vec(editor.lines);
                }
                if (key->code == sf::Keyboard::Key::V && key->control && !key->shift) { // Ctrl+V
                    const string clipboard = sf::Clipboard::getString().toAnsiString();
                    editor.lines[editor.cursorY].insert(editor.cursorX, clipboard);
                    editor.cursorX += clipboard.size();
                }
                tick = 0;
            }

            if (const auto* t = event->getIf<sf::Event::TextEntered>()) {
                if (t->unicode >= 32 && t->unicode < 127) {
                    char typed = static_cast<char>(t->unicode);

                    if      (typed == '('){editor.lines[editor.cursorY].insert(editor.cursorX, "()");}
                    else if (typed == '['){editor.lines[editor.cursorY].insert(editor.cursorX, "[]");}
                    else if (typed == '{'){editor.lines[editor.cursorY].insert(editor.cursorX, "{}");}
                    else if (typed == '"'){editor.lines[editor.cursorY].insert(editor.cursorX, "\"\"");}
                    else                  {editor.lines[editor.cursorY].insert(editor.cursorX, 1, typed);}
                    editor.cursorX++;

                    tick = 0;
                }
            }

            //////////////// Buttons ////////////////

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f pos((float)mouse->position.x, (float)mouse->position.y);

                if (mouse->button == sf::Mouse::Button::Left) {
                    for (auto& b : editor.buttons) b.handleClick(pos, false);
                } else if (mouse->button == sf::Mouse::Button::Right) {
                    for (auto& b : editor.buttons) b.handleClick(pos, true);
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
                        editor.offsetX -= (int)(mouse->delta * charw * 5 * mod);

                        int max_line_length = 0;
                        for(const auto&line:editor.lines) {
                            if((int)line.size()>max_line_length)max_line_length=line.size();
                        }
                        float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                        if (editor.offsetX < 0) editor.offsetX = 0;
                        if (editor.offsetX > max_scroll) editor.offsetX = max_scroll;

                    } else {
                        // Vertical
                        editor.offsetY -= (int)(mouse->delta * line_height * 3 * mod);
                        float max_scroll = std::max(0.f, editor.lines.size() * line_height - windowSize.y + 100.f);
                        if (editor.offsetY < 0) editor.offsetY = 0;
                        if (editor.offsetY > max_scroll) editor.offsetY = max_scroll;
                    }
                } 
                // Horizontal touchpad
                else if (mouse->wheel == sf::Mouse::Wheel::Horizontal) {
                    editor.offsetX -= (int)(mouse->delta * charw * 5 * mod);

                    int max_line_length = 0;
                    for(const auto&line:editor.lines)if((int)line.size()>max_line_length)max_line_length=line.size();
                    float max_scroll = std::max(0.f, max_line_length * charw - windowSize.x + 100.f);
                        
                    if (editor.offsetX < 0) editor.offsetX = 0;
                    if (editor.offsetX > max_scroll) editor.offsetX = max_scroll;
                }
            }
            if (editor.cursorX * charw > windowSize.x + editor.offsetX) editor.offsetX += charw;
        }

        gaps = charw * (to_string(editor.lines.size()).size()+1);

        cursor.setPosition({
            text_x + editor.cursorX * charw + gaps - editor.offsetX,
            text_y + editor.cursorY * line_height - editor.offsetY
        });

        if (theme == "dark") window.clear(sf::Color(30, 30, 30));
        else window.clear(sf::Color(255, 239, 223));

        for (size_t i = first_visible; i <= last_visible; i++) {
            text.setFillColor(HL(editor.lines[i], editor.save_filename, theme_color1));
            text.setString(editor.lines[i]);
            text.setPosition({text_x + gaps - editor.offsetX, text_y + i * line_height - editor.offsetY});
            
            window.draw(text);
        }
        
        window.draw(numsbg);

        for (size_t i = first_visible; i <= last_visible; i++) {
            strnums.setString(to_string(i+1));
            sf::FloatRect bounds = strnums.getLocalBounds();
            strnums.setOrigin({bounds.position.x + bounds.size.x + (charw * 1.5f), 0.f});
            strnums.setPosition({text_x + gaps, text_y + i * line_height - editor.offsetY});

            window.draw(strnums);
        }
        
        if (tick < fps/2) window.draw(cursor);
        else if (tick > fps-1) tick = 0;        

        for (auto& b : editor.buttons) window.draw(b);

        window.display();

        old_gaps = gaps;
    }
    if (update_handle != nullptr) wrenReleaseHandle(vm, update_handle);
    wrenFreeVM(vm);
    return 0;
}