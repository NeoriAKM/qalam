# Qalam - Text editor

My first C++ project. A lightweight text editor written in C++ using SFML.

The name comes from the Arabic word قلم (qalam), meaning "pen".

![](img/img1.png)

## Features
- Basic work with lines, like any other text editor
- Saving files
- Custom font, font size, and theme via config file
- Dark and light themes

## Dependencies
- C++17 compiler (GCC/Clang)
- CMake (3.16+)
- [SFML 3](https://www.sfml-dev.org/) (Graphics, Window, System)
- [nlohmann/json](https://github.com/nlohmann/json)

*On Arch Linux:* `sudo pacman -S cmake sfml nlohmann-json`

## Build

```bash
cmake -B build # once
cmake --build build
```

## Run

```bash
./build/qalam <filename>
```

## Flags

You can use some flags.
- `--help` or `-h`: for reference
- `--debug` or `-d`: for debug msgs in shell

## Configuration

config file is located at `~/.config/qalam/config.json`. Sorry, windows users, I don't know how to make configs for windows.

Standart config:

```json
{
    "font": "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
    "font_size": 20,
    "theme": "dark"
}
```

- `"font_size"` - any int-typed value
- `"font"` - absolute path to your font
- `"theme"` - `"dark"` or `"light"`

---

Copyright NeoriAKM(c) | made with love and C++