set(HEADERS
	GraphTest.h
	GraphCmdTest.h)

set(SOURCE
	GraphTest.cpp
	GraphCmdTest.cpp)

add_library(ibisRenderTest ${SOURCE} ${HEADERS})
target_link_libraries(ibisRenderTest ibisTestLib ibisRender)
set_target_properties(ibisRenderTest PROPERTIES FOLDER tests)
