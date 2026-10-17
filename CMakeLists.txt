cmake_minimum_required(VERSION 3.16)
project(qalam CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(qalam
    src/main.cpp
    src/saving.cpp
    src/config.cpp
)
target_link_libraries(qalam PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
)