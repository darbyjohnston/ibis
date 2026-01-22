set(HEADERS
    Bindings.h)
set(HEADERS_PRIVATE)
set(SOURCE
    Bindings.cpp
    Document.cpp
    DocumentModel.cpp)

add_library(ibisModelsPy ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(ibisModelsPy ibisModels Python3::Python)
set_target_properties(ibisModelsPy PROPERTIES FOLDER lib)
set_target_properties(ibisModelsPy PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisModelsPy
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisModelsPy)
