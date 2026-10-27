// Syntax highlight for Qalam
// Copyright (C) NeoriAKM

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include <SFML/Graphics/Color.hpp>

using namespace std;

bool here(const std::string& str, const std::string& val) {
    return str.find(val) != std::string::npos;
}

template <typename Container>
bool here(const Container& cont, const std::string& val) {
    return std::find(cont.begin(), cont.end(), val) != cont.end();
}

static string lower(string str) {
    transform(str.begin(), str.end(), str.begin(), [](unsigned char chr){ return std::tolower(chr); });
    return str;
}

string get_ext(string filename) {
    size_t slash = filename.find_last_of("/\\");
    if (slash != string::npos) filename = filename.substr(slash + 1);

    size_t dot = filename.rfind('.');
    if (dot == string::npos || dot == filename.size() - 1) return "";

    return lower(filename.substr(dot + 1));
}

sf::Color is_comment(string line, string filename, sf::Color non) {
    if (line.empty()) return non;
    line.erase(remove(line.begin(), line.end(), ' '), line.end());
    if (line.empty()) return non;

    sf::Color yeah = {120, 120, 120};
    static const vector<string> slashslash = {
        // C-family
        "c", "h",
        "cpp", "hpp", "cxx", "hxx", "cc", "hh", "cppm", "ixx",
        // JVM / .NET
        "java", "kt", "kts", "scala", "groovy", "cs",
        // Apple
        "swift", "m", "mm",
        // Web
        "js", "mjs", "cjs", "jsx", "ts", "tsx",
        "php", "scss", "less",
        // Other
        "go", "rs", "d", "zig", "v", "vala", "cr",
        "pas", "pp", "sol", "glsl", "hlsl", "vert", "frag",
        "cu", "cuh", "cl"
    };
    static const vector<string> hash = {
        // Python
        "py", "pyw", "pyi",
        // Ruby
        "rb", "rake", "gemspec",
        // Shell
        "sh", "bash", "zsh", "fish", "ksh",
        // Configs
        "yaml", "yml", "toml",
        // Other
        "pl", "pm", "r", "nim", "cr", "jl",
        "ex", "exs", "coffee", "ps1", "psm1",
        "tcl", "awk", "sed", "cmake", "dockerfile",
        "gitignore", "gitattributes", "gitmodules"
    };
    static const vector<string> dashdash = {
        "lua",  "hs",   "lhs",
        "sql",  "adb",  "ads",
        "vhd",  "vhdl", "elm",
        "m",    "applescript",
        "purs","idr", "agda"
    };
    static const vector<string> bang = {
        "f",   "for", "f77",
        "f90", "f95", "f03",
        "f08", "ftn"
    };

    string ext = get_ext(filename);

    if (here(slashslash, ext)) {
        if (line[0] == '/' && line[1] == '/') return yeah; // comment is //
    } else if (here(hash, ext)) {
        if (line[0] == '#') return yeah;                   // comments is #
    } else if (here(dashdash, ext)) {
        if (line[0] == '-' && line[1] == '-') return yeah; // comment is --
    } else if (ext == "asm") {
        if (line[0] == ';') return yeah;                   // comment is ';'
    }
    return non;
}

bool is_label(string line, string filename) { // Labels HL for assembler
    if (line.empty()) return false;
    if (line.back() == ':' && get_ext(filename) == "asm") return true;
    return false;
}

sf::Color cmd_asm(string line, string filename, sf::Color non) {
    if (line.empty() || get_ext(filename) != "asm") return non;

    static const vector<string> no_arg_instr = {
        "nop", "hlt", "pause", "leave", "retn", "retf",
        "iret", "iretd", "iretq", "sysret", "sysexit",
        "cpuid", "rdtsc", "rdtscp", "rdpmc",
        "rdmsr", "wrmsr", "clts", "swapgs",
        "lfence", "sfence", "mfence",
        "ud2", "int3", "int1", "into", "icebp",
        "xlatb", "xlat",
        "lahf", "sahf",
        "pushf", "popf", "pushfd", "popfd", "pushfq", "popfq",
        "pusha", "popa", "pushad", "popad",
        "cld", "std", "cli", "sti", "cmc", "clc", "stc",
        "cwd", "cdq", "cqo", "cbw", "cwde", "cdqe",
        "aaa", "aas", "aam", "aad", "daa", "das",
        "wait", "fwait",
        "wbinvd", "invd", "rsm", "getsec",
        "skinit", "stgi", "clgi",
        "vmcall", "vmlaunch", "vmresume", "vmxoff", "vmfunc",
        "xend", "xtest", "serialize",
        "endbr32", "endbr64",
        "salc", "lodsb",
        "fnop", "fchs", "fabs", "fsqrt",
        "fsin", "fcos", "fsincos",
        "fptan", "fpatan",
        "f2xm1", "fyl2x", "fyl2xp1",
        "fld1", "fldz", "fldpi", "fldl2e", "fldl2t", "fldlg2", "fldln2",
        "fprem", "fprem1", "fscale", "frndint", "fxtract",
        "ftst", "fxam",
        "fincstp", "fdecstp",
        "finit", "fninit", "fclex", "fnclex",
        "fcompp", "fucompp",
        "emms", "vzeroupper", "vzeroall"
    };

    static const vector<string> spec = {
        "push",  "pop",  "inc",  "dec",
        "section","jmp", "je",   "jne",
        "jc",    "jnc",  "jz",  "jnz",
        "call", 
    };

    static const vector<string> AL = {
        "or", "xor", "and", "not",
        "add", "sub", "mul", "div",
        "test"
    };

    static const vector<string> other = {
        "ret", "int"
    };

    size_t i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;
    size_t j = i;
    while (j < line.size() && (std::isalnum((unsigned char)line[j]) || line[j] == '_')) j++;
    std::string mnemonic = lower(line.substr(i, j - i));

    if (here(no_arg_instr, mnemonic))   return sf::Color(100, 100, 255);

    if (here(spec, mnemonic))           return sf::Color(100, 200, 200);
    if (here(other, mnemonic))          return sf::Color(50, 255, 50);
    if (here(AL, mnemonic))              return sf::Color(200, 0, 255);

    return non;
}

bool is_macro(string line, string filename) {
    if (line.empty()) return false;

    static const vector<string> hash = {
        "c", "cpp", "cxx",
        "cc","hpp", "cxx",
        "hh","h"
    };

    string ext = get_ext(filename);
    line.erase(remove(line.begin(), line.end(), ' '), line.end());

    if (line[0] == '%' && ext == "asm") return true;
    if (here(hash, ext) && line[0] == '#') return true;
    return false;
}

sf::Color HL(string line, string filename, sf::Color non) {
    if (line.empty()) return non;

    sf::Color red = sf::Color(255, 100, 100);
    sf::Color comment = is_comment(line, filename, non);

    if (comment != non) {
        return comment;
    } else {
        if (get_ext(filename) == "asm") {
            if (is_label(line, filename)) {return sf::Color(255, 255, 0);}
            else if (is_macro(line, filename)) {return sf::Color(255, 30, 30);}
            return cmd_asm(line, filename,  non);
        }
        if (is_macro(line, filename)) {return red;}
        if (here(line, "return")) {return red;}
    }
    return non;
}