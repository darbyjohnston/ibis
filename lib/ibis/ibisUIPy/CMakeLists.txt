set(HEADERS
    Bindings.h)
set(HEADERS_PRIVATE)
set(SOURCE
    Bindings.cpp)

add_library(ibisUIPy ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(ibisUIPy ibisUI Python3::Python)
set_target_properties(ibisUIPy PROPERTIES FOLDER lib)
set_target_properties(ibisUIPy PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisUIPy
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisUIPy)
