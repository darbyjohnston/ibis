set(HEADERS
    CompNodeWidget.h
    GenNodeWidget.h
    INodeWidget.h
    InputNodeWidget.h
    MathNodeWidget.h
    NodeBrowser.h
    NodeDragDrop.h
    NodeEditor.h
    NodeGraphCanvas.h
    NodeGraphWidget.h
    NodeWidgetFactory.h
    TimelineWidget.h
    Viewport.h)

set(SOURCE
    CompNodeWidget.cpp
    GenNodeWidget.cpp
    INodeWidget.cpp
    InputNodeWidget.cpp
    MathNodeWidget.cpp
    NodeBrowser.cpp
    NodeDragDrop.cpp
    NodeEditor.cpp
    NodeGraphCanvas.cpp
    NodeGraphWidget.cpp
    NodeWidgetFactory.cpp
    TimelineWidget.cpp
    Viewport.cpp)

set(LIBRARIES ibisModels ftk::ftkUI)

add_library(ibisUI ${HEADERS} ${SOURCE})
target_link_libraries(ibisUI ${LIBRARIES})
set_target_properties(ibisUI PROPERTIES FOLDER lib)
set_target_properties(ibisUI PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisUI
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisUI)
