set(HEADERS)

set(SOURCE
    Empty.cpp)

set(LIBRARIES ftk::ftkUI)

add_library(ibisUI ${HEADERS} ${SOURCE})
target_link_libraries(ibisUI ${LIBRARIES})
set_target_properties(ibisUI PROPERTIES FOLDER lib)
set_target_properties(ibisUI PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisUI
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisUI)
