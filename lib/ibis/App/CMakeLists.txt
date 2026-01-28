set(HEADERS
    App.h
    DocumentWidget.h
    EditActions.h
    EditMenu.h
    FileActions.h
    FileMenu.h
    MainWindow.h
    TimeActions.h
    TimeMenu.h
    ViewActions.h
    ViewMenu.h
    WindowActions.h
    WindowMenu.h)

set(SOURCE
    App.cpp
    DocumentWidget.cpp
    EditActions.cpp
    EditMenu.cpp
    FileActions.cpp
    FileMenu.cpp
    MainWindow.cpp
    TimeActions.cpp
    TimeMenu.cpp
    ViewActions.cpp
    ViewMenu.cpp
    WindowActions.cpp
    WindowMenu.cpp)

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
