set(HEADERS
    Document.h
    DocumentModel.h
    NodeSelectionModel.h
    TimeUnitsModel.h)

set(SOURCE
    Document.cpp
    DocumentModel.cpp
    NodeSelectionModel.cpp
    TimeUnitsModel.cpp)

set(LIBRARIES ibisRender)

add_library(ibisModels ${HEADERS} ${SOURCE})
target_link_libraries(ibisModels ${LIBRARIES})
set_target_properties(ibisModels PROPERTIES FOLDER lib)
set_target_properties(ibisModels PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisModels
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisModels)
