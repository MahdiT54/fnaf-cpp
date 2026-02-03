cmake_minimum_required(VERSION 3.16)

project(ConsoleFNAF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)

# Define executable ONCE
add_executable(console_fnaf
    src/clone.cpp
)

# Platform-specific configuration
if(APPLE OR UNIX)
    # macOS/Linux: use system ncurses
    target_link_libraries(console_fnaf PRIVATE ${CURSES_LIBRARIES})
    target_include_directories(console_fnaf PRIVATE ${CURSES_INCLUDE_DIRS})
elseif(WIN32)
    # Windows/MSYS2: use ncursesw
    target_include_directories(console_fnaf PRIVATE
        C:/msys64/ucrt64/include
    )
    target_link_libraries(console_fnaf PRIVATE
        ncursesw
    )
endif()
