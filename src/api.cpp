extern "C" {
    #include <wren.h>
}
#include <iostream>
// #include "inc/helper.hpp"

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
/*
static void getcursorstr(WrenVM* vm) {
    Editor* e = static_cast<Editor*>(wrenGetSlotForeign(vm, 0));
    if (!e) {wrenSetSlotString(vm, 0, ""); return;}

    if (e->cursorY >= e->lines.size()) {
        wrenSetSlotString(vm, 0, "");
        return;
    }

    const std::string& s = e->lines[e->cursorY];

    wrenSetSlotString(vm, 0, s.c_str());
}

static WrenForeignMethodFn bindEditorMethods(
    WrenVM* vm, const char*  module, const char* className,
    bool isStatic, const char* signature)
{
    if (std::string(className) == "Editor" && std::string(signature) == "cursorstr()") return getcursorstr;
    return nullptr;
}
*/