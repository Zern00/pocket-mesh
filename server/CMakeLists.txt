add_executable(pocketscene_server src/main.cpp)
set_target_properties(pocketscene_server PROPERTIES OUTPUT_NAME "pocketscene-server")

target_link_libraries(pocketscene_server PRIVATE PocketScene::ProjectOptions)
if(TARGET pocketscene_renderer)
    target_link_libraries(pocketscene_server PRIVATE PocketScene::Renderer)
endif()
pocketscene_enable_warnings(pocketscene_server)
