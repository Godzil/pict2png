cmake_minimum_required(VERSION 3.10)
project(Pict2Png)

add_subdirectory(external/lodepng)

add_executable(pict2png)
target_sources(pict2png PRIVATE main.c)
target_link_libraries(pict2png PRIVATE lodepng)
