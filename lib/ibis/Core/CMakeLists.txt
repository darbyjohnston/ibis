set(HEADERS
    Time.h)

set(SOURCE
    Time.cpp)

set(LIBRARIES ibisResource OTIO::opentime ftk::ftkCore)

add_library(ibisCore ${HEADERS} ${SOURCE})
target_link_libraries(ibisCore ${LIBRARIES})
set_target_properties(ibisCore PROPERTIES FOLDER lib)
set_target_properties(ibisCore PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisCore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisCore)
