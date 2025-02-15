cmake_minimum_required(VERSION 3.10)  # Or a more recent version
project(StarWarsTargetGame C)

set(CMAKE_C_STANDARD 11) # or c99 if you prefer

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(CAIRO REQUIRED cairo)

if (GTK_FOUND AND CAIRO_FOUND)
    include_directories(${GTK_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})
    link_directories(${GTK_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS})
    add_executable(starwars_target_game main.c) # Replace main.c with your source file name
    target_link_libraries(starwars_target_game ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} m)
else()
    message(FATAL_ERROR "GTK+ or Cairo not found.  Make sure they are installed.")
endif()
