#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

using namespace std;

void help_msg() {
    cout << "+-------------------- help msg ----------------------+\n";
    cout << "| left  click to button - quick save                 |\n";
    cout << "| right click to button - choose path to save        |\n";
    cout << "|                                                    |\n";
    cout << "|          read about config in README.md            |\n";
    cout << "+----------------------------------------------------+\n";
    cout << "| qalam <file>  -  open this file with qalam         |\n";
    cout << "+-------------------- hotkeys -----------------------+\n";
    cout << "| Ctrl+S        -  quick save                        |\n";
    cout << "| Ctrl+Shift+S  -  save with path choice             |\n";
    cout << "|                                                    |\n";
    cout << "| Ctrl+C        -  copying string under cursor       |\n";
    cout << "| Ctrl+Shift+C  -  copying ALL file to clipbaord     |\n";
    cout << "|                                                    |\n";
    cout << "| Ctrl+U        -  moving cursor to center of screen |\n";
    cout << "+---------------------- flags -----------------------+\n";
    cout << "| --help    -h        - opening this reference       |\n";
    cout << "| --version -v        - opening copyright text       |\n";
    cout << "| --debug   -d <file> - turning on debug mode        |\n";
    cout << "+----------------------------------------------------+\n\n";
    cout << "Good luck for your future projects with(out) Qalam!\n";
    return;
}

void copyright_msg() {
    cout << "+-----------------------------------+\n";
    cout << "|     Copyright © NeoriAKM (2026)   |\n";
    cout << "| All right reserved by MIT license |\n";
    cout << "|        More in file LICENSE       |\n";
    cout << "+-----------------------------------+\n";
    return;
}

size_t buflen(size_t line, const vector<string>& lines) {return lines[line].size();}


string repeat(const string& str, size_t times) {
    string result;
    result.reserve(str.length() * times);
    for (size_t i = 0; i < times; ++i) {
        result += str;
    }
    return result;
}

void copy_string(vector<string>& lines, size_t strnum) {
    if (lines.empty()) return;
    sf::Clipboard::setString(sf::String(lines[strnum]));
}

void copy_vec(vector<string>& lines) {
    if (lines.empty()) return;

    string result = accumulate(
        next(lines.begin()), lines.end(), lines[0],
        [](const string& a, const string& b) {return a + "\n" + b;}
    );

    sf::Clipboard::setString(sf::String(result));
}