cmake_minimum_required(VERSION 3.15)
project(cmark_watcher LANGUAGES C)

add_subdirectory(extern/cmark)

set(CMAKE_BUILD_TYPE Debug)

add_executable(cmark_watcher cmark_watcher.c)
target_include_directories(cmark_watcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/cmark/src)
target_link_libraries(cmark_watcher cmark)
