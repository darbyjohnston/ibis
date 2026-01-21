pybind11_add_module(ibisPy Bindings.cpp)
target_link_libraries(ibisPy PRIVATE ibisUIPy ibisRenderPy ibisModelsPy)
set_target_properties(ibisPy PROPERTIES FOLDER lib)
set_target_properties(ibisPy PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisPy
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
