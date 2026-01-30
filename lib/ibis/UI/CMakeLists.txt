set(HEADERS
    ColorNodeWidget.h
    CompNodeWidget.h
    DiagWidget.h
    FrameToolBar.h
    GenNodeWidget.h
    INodeWidget.h
    Init.h
    InputNodeWidget.h
    MathNodeWidget.h
    NodeBrowser.h
    NodeDragDrop.h
    NodeEditor.h
    NodeGraphCanvas.h
    NodeGraphWidget.h
    NodeWidgetFactory.h
    PlaybackLoopWidget.h
    PlaybackToolBar.h
    SettingsWidget.h
    SysLogWidget.h
    TimeEdit.h
    TimeLabel.h
    TimeSlider.h
    TimeUnitsWidget.h
    TimelineWidget.h
    Viewport.h)

set(SOURCE
    ColorNodeWidget.cpp
    CompNodeWidget.cpp
    DiagWidget.cpp
    FrameToolBar.cpp
    GenNodeWidget.cpp
    INodeWidget.cpp
    Init.cpp
    InputNodeWidget.cpp
    MathNodeWidget.cpp
    NodeBrowser.cpp
    NodeDragDrop.cpp
    NodeEditor.cpp
    NodeGraphCanvas.cpp
    NodeGraphWidget.cpp
    NodeWidgetFactory.cpp
    PlaybackLoopWidget.cpp
    PlaybackToolBar.cpp
    SettingsWidget.cpp
    SysLogWidget.cpp
    TimeEdit.cpp
    TimeLabel.cpp
    TimeSlider.cpp
    TimeUnitsWidget.cpp
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
