cmake_minimum_required(VERSION 3.16)
project(qalam CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(qalam
    src/main.cpp
    src/dialogues.cpp
    src/config.cpp
    src/syntax_hl.cpp
)

# target_compile_options(qalam PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
# target_link_options(qalam PRIVATE -fsanitize=address)

target_compile_options(qalam PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wreorder -Wunused-variable
)
set_source_files_properties(src/dialogues.cpp PROPERTIES COMPILE_OPTIONS "-Wsign-conversion")

target_link_libraries(qalam PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
)