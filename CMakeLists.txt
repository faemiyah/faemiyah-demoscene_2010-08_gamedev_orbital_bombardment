cmake_minimum_required(VERSION 2.6)

project(orbital_bombardment)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost_filesystem(TRUE)
find_boost_program_options(TRUE)
find_boost_system(TRUE)
find_boost_thread(TRUE)
find_glew(TRUE STATIC)
find_jpeg(TRUE STATIC)
find_ogg(TRUE STATIC)
find_openal(TRUE STATIC)
find_opengl(TRUE)
find_png(TRUE STATIC)
find_sdl(TRUE)
find_vorbis(TRUE STATIC)

output_flags("DEBUG" on)

add_definitions(-DVERSION="r62")

add_subdirectory(src)
