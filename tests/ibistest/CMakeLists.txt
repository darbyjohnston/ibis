set(HEADERS)

set(SOURCE
    main.cpp)

set(LIBRARIES ibisUITest ibisModelsTest ibisRenderTest ibisCoreTest)

add_executable(ibistest ${SOURCE} ${HEADERS})
target_link_libraries(ibistest ${LIBRARIES})
set_target_properties(ibistest PROPERTIES FOLDER tests)

add_test(ibistest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ibistest${CMAKE_EXECUTABLE_SUFFIX})
