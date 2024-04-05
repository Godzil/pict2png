cmake_minimum_required(VERSION 3.11)
project(LodePNG)

add_library(lodepng lodepng.c lodepng.h)
target_include_directories(lodepng PUBLIC .)