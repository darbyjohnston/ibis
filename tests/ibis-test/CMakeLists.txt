add_executable(ibis-test main.cpp)
target_link_libraries(ibis-test ibisUITest ibisModelsTest ibisRenderTest ibisCoreTest)
set_target_properties(ibis-test PROPERTIES FOLDER tests)

add_test(ibis-test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ibis-test${CMAKE_EXECUTABLE_SUFFIX})
