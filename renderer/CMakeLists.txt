add_library(
    pocketscene_renderer
    STATIC
        src/rid.cpp
)
add_library(PocketScene::Renderer ALIAS pocketscene_renderer)

target_include_directories(
    pocketscene_renderer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(pocketscene_renderer PUBLIC PocketScene::ProjectOptions)
pocketscene_enable_warnings(pocketscene_renderer)

if(BUILD_TESTING)
    add_executable(pocketscene_renderer_tests tests/rid_test.cpp)
    target_link_libraries(
        pocketscene_renderer_tests
        PRIVATE
            PocketScene::Renderer
            PocketScene::ProjectOptions
    )
    pocketscene_enable_warnings(pocketscene_renderer_tests)
    add_test(NAME renderer.rid COMMAND pocketscene_renderer_tests)
endif()
