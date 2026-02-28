set(HEADERS
    ColorNode.h
    CompNode.h
    GenNode.h
    Graph.h
    GraphCmd.h
    INode.h
    InputNode.h
    MathNode.h
    NodeFactory.h
    OutputNode.h
    RenderUtil.h
    TransformNode.h)

set(SOURCE
    ColorNode.cpp
    CompNode.cpp
    GenNode.cpp
    Graph.cpp
    GraphCmd.cpp
    INode.cpp
    InputNode.cpp
    MathNode.cpp
    NodeFactory.cpp
    OutputNode.cpp
    RenderUtil.cpp
    TransformNode.cpp)
if(ibis_OIIO)
    list(APPEND SOURCE InputNodeOIIO.cpp)
    list(APPEND SOURCE OutputNodeOIIO.cpp)
endif()
if(ibis_USD)
    list(APPEND HEADERS USDRender.h)
    list(APPEND SOURCE InputNodeUSD.cpp USDRender.cpp)
endif()

set(LIBRARIES ibisCore ftk::ftkUI ftk::ftkGL)
set(LIBRARIES_PRIVATE)
if(ibis_OIIO)
    list(APPEND LIBRARIES_PRIVATE OpenImageIO::OpenImageIO)
    if(ibis_TIFF AND BUILD_SHARED_LIBS)
        # \bug OIIO link bug?
        list(APPEND LIBRARIES_PRIVATE TIFF::TIFF)
    endif()
endif()
if(ibis_USD)
    list(APPEND LIBRARIES_PRIVATE tf usd usdGeom usdImagingGL usdAppUtils hd)
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
