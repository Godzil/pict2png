cmake_minimum_required(VERSION 3.10)
project(Pict2Png)

add_subdirectory(external/lodepng)

add_executable(pict2png)

set(MAX_LOG_LEVEL "NORMAL" CACHE STRING "Maximum output log level")
set(LOG_LEVEL "NORMAL" CACHE STRING "Default output log level")

target_compile_options(pict2png PRIVATE -DMAXIMUM_DEBUG_LEVEL=TLOG_${MAX_LOG_LEVEL})
target_compile_options(pict2png PRIVATE -DDEFAULT_DEBUG_LEVEL=TLOG_${LOG_LEVEL})


target_sources(pict2png PRIVATE main.c log.c log.h binfile.c binfile.h)
target_include_directories(pict2png PRIVATE .)
target_link_libraries(pict2png PRIVATE lodepng)
