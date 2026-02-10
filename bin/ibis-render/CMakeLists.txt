add_executable(ibis-render main.cpp)
target_link_libraries(ibis-render ibisRenderApp)
set_target_properties(ibis-render PROPERTIES FOLDER bin)

install(
    TARGETS ibis-render
    RUNTIME DESTINATION bin)
