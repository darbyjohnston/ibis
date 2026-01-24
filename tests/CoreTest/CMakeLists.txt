set(HEADERS
	TimeTest.h)

set(SOURCE
	TimeTest.cpp)

add_library(ibisCoreTest ${SOURCE} ${HEADERS})
target_link_libraries(ibisCoreTest ibisTestLib ibisCore)
set_target_properties(ibisCoreTest PROPERTIES FOLDER tests)
