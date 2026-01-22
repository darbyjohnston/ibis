set(HEADERS
    Bindings.h)
set(HEADERS_PRIVATE)
set(SOURCE
    Bindings.cpp)

add_library(ibisRenderPy ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(ibisRenderPy ibisRender Python3::Python)
set_target_properties(ibisRenderPy PROPERTIES FOLDER lib)
set_target_properties(ibisRenderPy PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisRenderPy
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisRenderPy)
