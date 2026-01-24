set(HEADERS)

set(SOURCE Empty.cpp)

add_library(ibisUITest ${SOURCE} ${HEADERS})
target_link_libraries(ibisUITest ibisTestLib ibisUI)
set_target_properties(ibisUITest PROPERTIES FOLDER tests)
