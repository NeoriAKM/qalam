cmake_minimum_required(VERSION 3.16)
project(qalam CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
find_package(nlohmann_json REQUIRED)

############# wren #############
find_path(WREN_INCLUDE_DIR wren.h)
find_library(WREN_LIBRARY NAMES wren)
if(NOT WREN_INCLUDE_DIR OR NOT WREN_LIBRARY)
    message(FATAL_ERROR "Wren not found! Please install it or specify paths manually.")
endif()

add_executable(qalam
    src/main.cpp
    src/dialogues.cpp
    src/config.cpp
    src/syntax_hl.cpp
    src/helper.cpp
    src/api.cpp
)

# target_compile_options(qalam PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
# target_link_options(qalam PRIVATE -fsanitize=address)

target_include_directories(qalam PRIVATE ${WREN_INCLUDE_DIRS})

target_compile_options(qalam PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wreorder -Wunused-variable -Wsign-conversion
)

target_link_libraries(qalam PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
    ${WREN_LIBRARY}
)