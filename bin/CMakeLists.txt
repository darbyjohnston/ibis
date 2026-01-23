set(HEADERS
    App.h
    DocumentWidget.h
    EditActions.h
    EditMenu.h
    FileActions.h
    FileMenu.h
    MainWindow.h)

set(SOURCE
    App.cpp
    DocumentWidget.cpp
    EditActions.cpp
    EditMenu.cpp
    FileActions.cpp
    FileMenu.cpp
    MainWindow.cpp
    main.cpp)

add_executable(ibis ${HEADERS} ${SOURCE})
target_link_libraries(ibis ibisUI)
set_target_properties(ibis PROPERTIES FOLDER bin)

install(
    TARGETS ibis
    RUNTIME DESTINATION bin)
