cmake_minimum_required(VERSION 3.16)

project(ConsoleFNAF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)

# Define executable ONCE
add_executable(console_fnaf
    src/main.cpp
    src/game_state.cpp
    src/ai.cpp
    src/input.cpp
    src/render.cpp
    src/audio.cpp
)

add_custom_command(TARGET console_fnaf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:console_fnaf>/assets"
)

# Platform-specific configuration
if(APPLE OR UNIX)
    # macOS/Linux: use system ncurses
    target_link_libraries(console_fnaf PRIVATE ${CURSES_LIBRARIES})
    target_include_directories(console_fnaf PRIVATE ${CURSES_INCLUDE_DIRS})
elseif(WIN32)
    # Windows/MSYS2: use ncursesw
    set(NCURSES_ROOT "C:/msys64/ucrt64" CACHE PATH "MSYS2 UCRT64 installation root")
    target_include_directories(console_fnaf PRIVATE
        "${NCURSES_ROOT}/include"
    )
    target_link_directories(console_fnaf PRIVATE
        "${NCURSES_ROOT}/lib"
    )
    target_link_libraries(console_fnaf PRIVATE
        ncursesw
        winmm
        ole32
        avrt
    )
endif()
