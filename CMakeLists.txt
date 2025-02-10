cmake_minimum_required(VERSION 3.10)
project(quid)

if(DEFINED ENV{HOME})
  set(HOME_DIR "$ENV{HOME}")
else()
  message(FATAL_ERROR "HOME environment variable not defined.")
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define include and library paths.  Replace with your actual paths.
set(PROJECT_INCLUDE_DIR "${HOME_DIR}/.local/include")
set(PROJECT_LIBRARY_DIR "${HOME_DIR}/.local/lib")

# Add include directory to the compiler's include path.
include_directories(${PROJECT_INCLUDE_DIR})

# Add libraries.
link_directories(${PROJECT_LIBRARY_DIR})

set(CGUI_LIBRARY "${HOME_DIR}/.local/lib/libcgui.so")
set(COBJ_LIBRARY "${HOME_DIR}/.local/lib/libcobj.so")
set(CGUI_INCLUDE_DIR "${HOME_DIR}/.local/include")

add_library(cgui SHARED IMPORTED) # SHARED specifies it's a dynamic library
set_target_properties(cgui PROPERTIES IMPORTED_LOCATION ${CGUI_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${CGUI_INCLUDE_DIR})

add_library(cobj SHARED IMPORTED)
set_target_properties(cobj PROPERTIES IMPORTED_LOCATION ${COBJ_LIBRARY})

# Add source file.
add_executable(quid src/main.c)
target_link_libraries(quid cgui cobj) # Assumed cobject is a typo for cobobj

# Set runtime library path and C23
set_target_properties(quid PROPERTIES
          LINKER_FLAGS "-Wl,-rpath,${PROJECT_LIBRARY_DIR}"
          C_STANDARD 23)
