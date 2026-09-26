add_library(
    pocketscene_jni
    SHARED
        native_renderer_bridge.cpp
)
set_target_properties(pocketscene_jni PROPERTIES OUTPUT_NAME "pocketscene-jni")

find_library(android_library android REQUIRED)
find_library(egl_library EGL REQUIRED)
find_library(gles_library GLESv3 REQUIRED)
find_library(log_library log REQUIRED)

target_link_libraries(
    pocketscene_jni
    PRIVATE
        PocketScene::ProjectOptions
        PocketScene::Renderer
        ${android_library}
        ${egl_library}
        ${gles_library}
        ${log_library}
)
pocketscene_enable_warnings(pocketscene_jni)
