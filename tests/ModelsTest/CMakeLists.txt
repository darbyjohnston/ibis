set(HEADERS
	DocumentTest.h
	DocumentModelTest.h
	NodeSelectionModelTest.h)

set(SOURCE
	DocumentTest.cpp
	DocumentModelTest.cpp
	NodeSelectionModelTest.cpp)

add_library(ibisModelsTest ${SOURCE} ${HEADERS})
target_link_libraries(ibisModelsTest ibisTestLib ibisModels)
set_target_properties(ibisModelsTest PROPERTIES FOLDER tests)
