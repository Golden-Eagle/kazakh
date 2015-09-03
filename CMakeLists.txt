
cmake_minimum_required (VERSION 3.1)

project (PXLJM15 CXX C)

list(APPEND CMAKE_MODULE_PATH "${GECOM_SOURCE_DIR}/cmake")

# enable IDE project folders
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# OpenGL
find_package(OpenGL REQUIRED)

# GECom
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/gecom")

# Stb
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/stb")

# Fmod
# add_subdirectory("${PROJECT_SOURCE_DIR}/ext/fmod")

# Imgui
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/imgui")

# Bullet
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/bullet-2.83.5" "${PROJECT_BINARY_DIR}/ext/bullet")

# Set GECOM default compile options
include(GEComCompileOptions)
gecom_add_default_compile_options()

# output directories
# this is hacky nasty shit and i hate it
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# source
add_subdirectory(src)

# resources - this is a custom target with no build step;
# primarily to allow shader editing in IDEs
add_subdirectory(res)

# set IDE display folders
SET_PROPERTY(TARGET pxljm PROPERTY FOLDER "PXLJM")
# SET_PROPERTY(TARGET fmod_lowlevel PROPERTY FOLDER "FMOD")
# SET_PROPERTY(TARGET fmod_studio PROPERTY FOLDER "FMOD")
SET_PROPERTY(TARGET stb PROPERTY FOLDER "STB")



