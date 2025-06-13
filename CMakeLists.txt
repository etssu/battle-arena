cmake_minimum_required(VERSION 3.30)
project(z6 C)

set(CMAKE_C_STANDARD 11)
find_package(Curses REQUIRED)

add_executable(z6 main.c graphics.c logic.c data.c)
target_link_libraries(z6 PRIVATE ${CURSES_LIBRARIES})
target_include_directories(z6 PRIVATE ${CURSES_INCLUDE_DIR})
