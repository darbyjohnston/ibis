set(HEADERS
    NodeBrowser.h
    NodeEditor.h
    NodeGraphCanvas.h
    NodeGraphWidget.h
    TimelineWidget.h
    Viewport.h)

set(SOURCE
    NodeBrowser.cpp
    NodeEditor.cpp
    NodeGraphCanvas.cpp
    NodeGraphWidget.cpp
    TimelineWidget.cpp
    Viewport.cpp)

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
