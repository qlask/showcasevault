cmake_minimum_required(VERSION 3.21)

# Nombre del proyecto - debe coincidir conceptualmente con mod.json
project(LevelExporter VERSION 1.0.0)

# Archivos fuente del mod
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

# --- Geode SDK ---
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable not set. Install the Geode CLI/SDK first.")
else()
    message(STATUS "Found Geode SDK: $ENV{GEODE_SDK}")
    set(GEODE_SDK_PATH $ENV{GEODE_SDK})
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

# --- SQLite embebido (amalgamation) ---
# El archivo third_party/sqlite3.c y sqlite3.h se descargan automaticamente
# via CMake FetchContent (ver abajo), asi que no hace falta commitearlos al repo.
include(FetchContent)
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450100.zip
)
FetchContent_MakeAvailable(sqlite3)

target_sources(${PROJECT_NAME} PRIVATE
    ${sqlite3_SOURCE_DIR}/sqlite3.c
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${sqlite3_SOURCE_DIR}
)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
endif()
