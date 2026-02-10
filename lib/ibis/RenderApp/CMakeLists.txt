set(HEADERS
    App.h)

set(SOURCE
    App.cpp)

add_library(ibisRenderApp ${HEADERS} ${SOURCE})
target_link_libraries(ibisRenderApp ibisRender)
set_target_properties(ibisRenderApp PROPERTIES FOLDER lib)
set_target_properties(ibisRenderApp PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisRenderApp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisRenderApp)
