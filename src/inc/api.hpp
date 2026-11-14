#pragma once
extern "C" {
    #include <wren.h>
}

void writeFn(WrenVM* /* vm */, const char* text);
void errorFn(
    WrenVM* /* vm */, WrenErrorType errorType,
    const char* module, const int line, const char* msg
);