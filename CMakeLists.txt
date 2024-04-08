cmake_minimum_required(VERSION 3.10)
project(Pict2Png)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake ${CMAKE_MODULE_PATH})

# Generate version from Git Tag
include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-dirty)

message("-- Building version ${VERSION}")
add_compile_options(-DAPP_VERSION="${VERSION}")

add_subdirectory(external/lodepng)

add_executable(pict2png)

set(MAX_LOG_LEVEL "NORMAL" CACHE STRING "Maximum output log level")
set(LOG_LEVEL "NORMAL" CACHE STRING "Default output log level")

target_compile_options(pict2png PRIVATE -DMAXIMUM_DEBUG_LEVEL=TLOG_${MAX_LOG_LEVEL})
target_compile_options(pict2png PRIVATE -DDEFAULT_DEBUG_LEVEL=TLOG_${LOG_LEVEL})


target_sources(pict2png PRIVATE main.c log.c log.h binfile.c binfile.h quickimagedraw.c quickimagedraw.h)
target_include_directories(pict2png PRIVATE .)
target_link_libraries(pict2png PRIVATE lodepng)
