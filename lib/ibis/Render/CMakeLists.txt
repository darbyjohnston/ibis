set(HEADERS
    CompNode.h
    Graph.h
    GraphCommands.h
    INode.h
    IONode.h
    MathNode.h
    NodeFactory.h)

set(SOURCE
    CompNode.cpp
    Graph.cpp
    GraphCommands.cpp
    INode.cpp
    IONode.cpp
    MathNode.cpp
    NodeFactory.cpp)

set(LIBRARIES ftk::ftkGL)

add_library(ibisRender ${HEADERS} ${SOURCE})
target_link_libraries(ibisRender ${LIBRARIES})
set_target_properties(ibisRender PROPERTIES FOLDER lib)
set_target_properties(ibisRender PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisRender
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisRender)
