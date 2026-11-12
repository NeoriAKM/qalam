extern "C" {
    #include <wren.h>
}
#include <iostream>
#include "inc/helper.hpp"

void writeFn(WrenVM* /* vm */, const char* text) {std::cout << text;}

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

std::string getcursorstr(WrenVM* /* wm */, Editor e) {return e.lines[e.cursorY];}