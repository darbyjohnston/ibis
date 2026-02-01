set(HEADERS
    App.h
    DocumentWidget.h
    EditActions.h
    EditMenu.h
    EditToolBar.h
    FileActions.h
    FileMenu.h
    FileToolBar.h
    MainWindow.h
    NodeMenu.h
    SidePanel.h
    TimeActions.h
    TimeMenu.h
    ViewActions.h
    ViewMenu.h
    ViewToolBar.h
    WindowActions.h
    WindowMenu.h
    WindowToolBar.h)

set(SOURCE
    App.cpp
    DocumentWidget.cpp
    EditActions.cpp
    EditMenu.cpp
    EditToolBar.cpp
    FileActions.cpp
    FileMenu.cpp
    FileToolBar.cpp
    MainWindow.cpp
    NodeMenu.cpp
    SidePanel.cpp
    TimeActions.cpp
    TimeMenu.cpp
    ViewActions.cpp
    ViewMenu.cpp
    ViewToolBar.cpp
    WindowActions.cpp
    WindowMenu.cpp
    WindowToolBar.cpp)

set(LIBRARIES ibisUI)

add_library(ibisApp ${HEADERS} ${SOURCE})
target_link_libraries(ibisApp ${LIBRARIES})
set_target_properties(ibisApp PROPERTIES FOLDER lib)
set_target_properties(ibisApp PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisApp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisApp)
