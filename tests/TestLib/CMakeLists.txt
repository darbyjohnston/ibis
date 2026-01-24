set(HEADERS
    ITest.h
    ITestInline.h)

set(SOURCE
    ITest.cpp)

add_library(ibisTestLib ${SOURCE} ${HEADERS})
target_link_libraries(ibisTestLib ibisCore)
set_target_properties(ibisTestLib PROPERTIES FOLDER tests)
