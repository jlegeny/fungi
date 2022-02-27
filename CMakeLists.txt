cmake_minimum_required(VERSION 3.5)

if(MINGW)
  set(CMAKE_C_COMPILER "i686-w64-mingw32-gcc")
  set(CMAKE_CXX_COMPILER "i686-w64-mingw32-g++")
elseif(WIN32)
  message(FATAL_ERROR "Only Cygwin supported on Windows")
endif()

project(Fungi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/usr/local/lib/pkgconfig")

# dependencies

find_package(PkgConfig REQUIRED)


if(APPLE)
  pkg_search_module(ALLEGRO REQUIRED allegro_main-5)
else()
  pkg_search_module(ALLEGRO REQUIRED allegro-5)
endif()

pkg_search_module(ALLEGRO_FONT REQUIRED allegro_font-5)
pkg_search_module(ALLEGRO_PRIMITIVES REQUIRED allegro_primitives-5)
pkg_search_module(ALLEGRO_TTF REQUIRED allegro_ttf-5)
pkg_search_module(ALLEGRO_IMAGE REQUIRED allegro_image-5)

# game stuff

add_library(core
    src/data.h
    src/util.h
    src/util.cpp
    )

add_library(game
    src/game.h
    src/game.cpp
    )

target_link_libraries(game core)

link_directories(${ALLEGRO_LIBRARY_DIRS})

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/renderer.cpp
    src/renderer.h
    src/controller.cpp
    src/controller.h
    )

include_directories(${PROJECT_NAME}
    ${ALLEGRO_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT_NAME}
    core
    game
    ${ALLEGRO_IMAGE_LIBRARIES}
    ${ALLEGRO_TTF_LIBRARIES}
    ${ALLEGRO_PRIMITIVES_LIBRARIES}
    ${ALLEGRO_FONT_LIBRARIES}
    ${ALLEGRO_LIBRARIES}
    )

