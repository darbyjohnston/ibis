add_executable(ibis main.cpp)
target_link_libraries(ibis ibisApp)
set_target_properties(ibis PROPERTIES FOLDER bin)

install(
    TARGETS ibis
    RUNTIME DESTINATION bin)
