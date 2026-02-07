cmake_minimum_required(VERSION 3.12)
project(CGFrameworkStudent)

# ----------------------------
# Paths
# ----------------------------
set(DIR_ROOT    ${CMAKE_CURRENT_LIST_DIR})
set(DIR_SOURCES ${DIR_ROOT}/src)

# ----------------------------
# OpenGL
# ----------------------------
find_package(OpenGL REQUIRED)
if(NOT TARGET OpenGL::GLU)
    message(FATAL_ERROR "GLU could not be found")
endif()

# ----------------------------
# Output directories
# ----------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ----------------------------
# External libraries
# ----------------------------
add_subdirectory(libraries/sdl2 EXCLUDE_FROM_ALL)
add_subdirectory(libraries/glew-cmake EXCLUDE_FROM_ALL)

# ----------------------------
# Source collection helpers
# ----------------------------
macro(CG_FILES_APPEND)
    file(GLOB FILES_APPEND CONFIGURE_DEPENDS ${ARGV})
    list(APPEND CG_SOURCES ${FILES_APPEND})
endmacro()

macro(CG_SOURCES_APPEND)
    CG_FILES_APPEND(${ARGV0}/*.h)
    CG_FILES_APPEND(${ARGV0}/*.cpp)
endmacro()

# ----------------------------
# Project sources
# ----------------------------
CG_SOURCES_APPEND(${DIR_SOURCES}/extra)
CG_SOURCES_APPEND(${DIR_SOURCES}/framework)
CG_SOURCES_APPEND(${DIR_SOURCES}/main)

add_executable(ComputerGraphics ${CG_SOURCES})

# ----------------------------
# Includes
# ----------------------------
target_include_directories(ComputerGraphics PUBLIC
    ${DIR_SOURCES}
)

# ----------------------------
# SDL2
# ----------------------------
target_link_libraries(ComputerGraphics PRIVATE SDL2 SDL2main)

# IMPORTANT:
# Put generated SDL_config include dirs FIRST (so we don't pick the wrong SDL_config.h)
target_include_directories(ComputerGraphics PRIVATE
    ${CMAKE_BINARY_DIR}/libraries/sdl2/include-config-debug
    ${CMAKE_BINARY_DIR}/libraries/sdl2/include-config-release
    ${CMAKE_BINARY_DIR}/libraries/sdl2/include-config-relwithdebinfo
    ${CMAKE_BINARY_DIR}/libraries/sdl2/include-config-minsizerel

    ${DIR_ROOT}/libraries/sdl2/include
)

set_property(TARGET SDL2 PROPERTY FOLDER "External/SDL2")
set_property(TARGET SDL2main PROPERTY FOLDER "External/SDL2")

# ----------------------------
# GLEW
# ----------------------------
add_definitions(-DGLEW_STATIC)
target_link_libraries(ComputerGraphics PRIVATE libglew_static)
set_property(TARGET libglew_static PROPERTY FOLDER "External/GLEW")

# ----------------------------
# OpenGL link
# ----------------------------
target_link_libraries(ComputerGraphics PRIVATE OpenGL::GL OpenGL::GLU)

# ----------------------------
# Compiler settings
# ----------------------------
set_property(TARGET ComputerGraphics PROPERTY CXX_STANDARD 11)
set_property(TARGET ComputerGraphics PROPERTY CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Windows-only definitions
# ----------------------------
if (NOT UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-D_AMD64_)
        message(STATUS "64 bits detected")
    else()
        add_definitions(-D_X86_)
        message(STATUS "32 bits detected")
    endif()
endif()

# ----------------------------
# Debug prints
# ----------------------------
message(STATUS "Root dir: ${DIR_ROOT}")
message(STATUS "Build dir: ${CMAKE_BINARY_DIR}")


