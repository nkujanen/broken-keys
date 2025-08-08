cmake_minimum_required(VERSION 3.15...4.0)

project(BrokenKeys VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
add_library(core
    src/core/PianoKey.cpp
    src/core/PianoKeyboard.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# UI library
add_library(ui
    src/ui/ObjectGrid.cpp
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Executable
add_executable(BrokenKeys WIN32
    src/main.cpp
)

target_link_libraries(BrokenKeys
    PRIVATE
        core
        ui
)

# Help compiler find entrypoint wWinMain
target_compile_options(BrokenKeys
    PRIVATE
        -municode
)

target_link_options(BrokenKeys
    PRIVATE
        -municode
)