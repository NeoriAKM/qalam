#include <iostream>
#include <vector>

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

void vector_msg(vector<string> vec) {
    for (size_t i = 0; i < vec.size(); i++) cout << vec[i];
    return;
}