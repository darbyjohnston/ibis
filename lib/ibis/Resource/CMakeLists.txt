set(HEADERS)
set(SOURCE)
set(RESOURCES
    etc/Icons/ArithmeticNode.svg
    etc/Icons/BrightnessNode.svg
    etc/Icons/ContrastNode.svg
    etc/Icons/DefaultNode.svg
    etc/Icons/ImageFileNode.svg
    etc/Icons/ImageSequenceNode.svg
    etc/Icons/NodeDragDrop.svg
    etc/Icons/NodePort.svg
    etc/Icons/NodePortConnected.svg
    etc/Icons/OverNode.svg
    etc/Icons/PlaybackLoop.svg
    etc/Icons/PlaybackOnce.svg
    etc/Icons/PlaybackPingPong.svg
    etc/Icons/SVGFileNode.svg
    etc/Icons/SolidColorNode.svg
    etc/Icons/View.svg
    etc/Icons/ibis.svg)
foreach(RESOURCE ${RESOURCES})
    get_filename_component(RESOURCE_BASE ${RESOURCE} NAME_WE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.cpp
        COMMAND $<TARGET_FILE:ftk::ftk-resource> ${CMAKE_SOURCE_DIR}/${RESOURCE} ${RESOURCE_BASE} ibis_resource
        DEPENDS ${CMAKE_SOURCE_DIR}/${RESOURCE})
    list(APPEND SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.cpp)
endforeach()

add_library(ibisResource ${HEADERS} ${SOURCE})
target_link_libraries(ibisResource)
set_target_properties(ibisResource PROPERTIES FOLDER lib)
set_target_properties(ibisResource PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(
    TARGETS ibisResource
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/tlRender/Resource)
