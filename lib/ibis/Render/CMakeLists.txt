set(HEADERS
    CompNode.h
    GenNode.h
    Graph.h
    GraphCmd.h
    GraphRender.h
    INode.h
    InputNode.h
    MathNode.h
    NodeFactory.h)

set(SOURCE
    CompNode.cpp
    GenNode.cpp
    Graph.cpp
    GraphCmd.cpp
    GraphRender.cpp
    INode.cpp
    InputNode.cpp
    MathNode.cpp
    NodeFactory.cpp)

set(LIBRARIES ibisCore ftk::ftkGL)

add_library(ibisRender ${HEADERS} ${SOURCE})
target_link_libraries(ibisRender ${LIBRARIES})
set_target_properties(ibisRender PROPERTIES FOLDER lib)
set_target_properties(ibisRender PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisRender
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisRender)
