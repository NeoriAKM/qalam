// Syntax highlight for Qalam
// Copyright (C) NeoriAKM


#include <string>
#include <vector>
#include <algorithm>

std::string get_ext(std::string filename) {
    size_t pos = filename.find('.');
    std::string ext;
    if (pos != std::string::npos) ext = filename.substr(pos + 1);
    return ext;
}

int is_comment(std::string line, std::string filename) {
    if (line.empty()) return 1;
    std::vector<std::string> slashslash = {
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
    std::vector<std::string> hash = {
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

    std::vector<std::string> dashdash = {
        "lua",  "hs",   "lhs",
        "sql",  "adb",  "ads",
        "vhd",  "vhdl", "elm",
        "m",    "applescript",
        "purs","idr", "agda"
    };

    std::vector<std::string> bang = {
        "f",   "for", "f77",
        "f90", "f95", "f03",
        "f08", "ftn"
    };

    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    std::string ext = get_ext(filename);

    if (std::find(slashslash.begin(), slashslash.end(), ext) != slashslash.end()) {
        if (line[0] == '/' && line[1] == '/') return 0; // comment is //
    } else if (std::find(hash.begin(), hash.end(), ext) != hash.end()) {
        if (line[0] == '#') return 0;                   // comments is #
    } else if (std::find(dashdash.begin(), dashdash.end(), ext) != dashdash.end()) {
        if (line[0] == '-' && line[1] == '-') return 0; // comment is --
    } else if (line[0] == ';') {
        if (line[0] == ';') return 0;                   // comment is ';'
    }
    return 1;
}

int is_label(std::string line, std::string filename) {
    if (line.empty()) return 1;
    if (line.back() == ':' && get_ext(filename) == "asm") {
        return 0;
    }
    return 1;
}