cmake_minimum_required(VERSION 3.25)

project(
    PocketScene
    VERSION 0.1.0
    DESCRIPTION "Mobile 3D asset inspector and collaborative review tool"
    LANGUAGES CXX
)

include(CTest)

option(POCKETSCENE_BUILD_RENDERER "Build the rendering core" ON)
option(POCKETSCENE_BUILD_SERVER "Build the backend server" ON)
option(POCKETSCENE_BUILD_ANDROID_BRIDGE "Build the Android JNI bridge" ${ANDROID})

add_library(pocketscene_project_options INTERFACE)
add_library(PocketScene::ProjectOptions ALIAS pocketscene_project_options)
target_compile_features(pocketscene_project_options INTERFACE cxx_std_20)

# Reusable target-level build policies live here instead of being copied into
# renderer, server, tests, and platform bridges.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(PocketSceneWarnings)

if(POCKETSCENE_BUILD_RENDERER)
    add_subdirectory(renderer)
endif()

if(POCKETSCENE_BUILD_SERVER)
    add_subdirectory(server)
endif()

if(ANDROID AND POCKETSCENE_BUILD_ANDROID_BRIDGE)
    add_subdirectory(android/app/src/main/cpp)
endif()
