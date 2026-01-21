set(HEADERS)

set(SOURCE
    Empty.cpp)

set(LIBRARIES ftk::ftkCore)

add_library(ibisModels ${HEADERS} ${SOURCE})
target_link_libraries(ibisModels ${LIBRARIES})
set_target_properties(ibisModels PROPERTIES FOLDER lib)
set_target_properties(ibisModels PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisModels
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisModels)
