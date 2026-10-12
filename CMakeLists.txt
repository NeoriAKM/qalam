cmake_minimum_required(VERSION 3.16)
project(qalam CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(qalam src/main.cpp)
target_link_libraries(qalam PRIVATE SFML::Graphics SFML::Window SFML::System)