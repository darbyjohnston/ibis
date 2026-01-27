set(HEADERS
    CompNode.h
    GenNode.h
    Graph.h
    GraphCmd.h
    INode.h
    InputNode.h
    MathNode.h
    NodeFactory.h
    RenderUtil.h)

set(SOURCE
    CompNode.cpp
    GenNode.cpp
    Graph.cpp
    GraphCmd.cpp
    INode.cpp
    InputNode.cpp
    MathNode.cpp
    NodeFactory.cpp
    RenderUtil.cpp)

set(LIBRARIES ibisCore ftk::ftkGL)
set(LIBRARIES_PRIVATE)
if(ibis_OIIO)
    list(APPEND LIBRARIES_PRIVATE OpenImageIO::OpenImageIO)
    if(ibis_TIFF AND BUILD_SHARED_LIBS)
        # \bug OIIO link bug?
        list(APPEND LIBRARIES_PRIVATE TIFF::TIFF)
    endif()
endif()

add_library(ibisRender ${HEADERS} ${SOURCE})
target_link_libraries(ibisRender PUBLIC ${LIBRARIES} PRIVATE ${LIBRARIES_PRIVATE})
set_target_properties(ibisRender PROPERTIES FOLDER lib)
set_target_properties(ibisRender PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ibisRender
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ibis/ibisRender)
