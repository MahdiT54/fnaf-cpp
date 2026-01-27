cmake_minimum_required(VERSION 3.16)

project(ConsoleFNAF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(console_fnaf
    src/main.cpp
)

# ncurses (MSYS2 / Windows)
target_include_directories(console_fnaf PRIVATE
    C:/msys64/ucrt64/include
)

target_link_libraries(console_fnaf PRIVATE
    ncursesw
)
