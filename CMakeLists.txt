#-------------------------------------------------------------------------------
# Project

cmake_minimum_required(VERSION 3.31)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0114 NEW)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(ibis_VERSION_MAJOR "0")
set(ibis_VERSION_MINOR "1")
set(ibis_VERSION_PATCH "0")
set(ibis_VERSION_DEV "-dev")
set(ibis_VERSION_FULL ${ibis_VERSION_MAJOR}.${ibis_VERSION_MINOR}.${ibis_VERSION_PATCH}${ibis_VERSION_DEV})
add_definitions(-DIBIS_VERSION_MAJOR=${ibis_VERSION_MAJOR})
add_definitions(-DIBIS_VERSION_MINOR=${ibis_VERSION_MINOR})
add_definitions(-DIBIS_VERSION_PATCH=${ibis_VERSION_PATCH})
add_definitions(-DIBIS_VERSION_FULL="${ibis_VERSION_FULL}")

project(
    ibis
    VERSION ${ibis_VERSION_MAJOR}.${ibis_VERSION_MINOR}.${ibis_VERSION_PATCH}
    DESCRIPTION "ibis compositor"
    HOMEPAGE_URL "https://github.com/darbyjohnston/ibis"
    LANGUAGES CXX C)

#-------------------------------------------------------------------------------
# Build Options

set(ftk_API "GL_4_1" CACHE STRING "Graphics API (GL_4_1, GL_4_1_Debug, GLES_2)")

set(ibis_OCIO ON CACHE BOOL "Enable support for OpenColorIO")
set(ibis_OIIO ON CACHE BOOL "Enable support for OpenImageIO")
set(ibis_USD OFF CACHE BOOL "Enable support for USD")
set(ibis_PYTHON ON CACHE BOOL "Enable support for Python")
set(ibis_TESTS ON CACHE BOOL "Build tests")
if(APPLE)
    set(ibis_IGNORE_PREFIX_PATH_DEFAULT /opt/homebrew)
endif()
set(ibis_IGNORE_PREFIX_PATH ${ibis_IGNORE_PREFIX_PATH_DEFAULT} CACHE STRING "Ignore the given prefix path")
set(ibis_GCOV OFF CACHE BOOL "Enable gcov code coverage")
set(ibis_GPROF OFF CACHE BOOL "Enable gprof code profiling")

#-------------------------------------------------------------------------------
# Internal Configuration

list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
endif()

if(NOT CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(ibis_IGNORE_PREFIX_PATH)
    set(CMAKE_IGNORE_PREFIX_PATH ${ibis_IGNORE_PREFIX_PATH})
endif()

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    add_definitions(-DFTK_ASSERT)
endif()

if(ibis_TESTS)
    set(CTEST_OUTPUT_ON_FAILURE ON)
    enable_testing()
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/deps
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/etc
    ${PROJECT_SOURCE_DIR}/tests)

if(ibis_GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

if(ibis_GCOV)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-update=atomic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-update=atomic")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -fprofile-update=atomic")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

if(APPLE)
    set(CMAKE_FIND_FRAMEWORK LAST)
endif()

if(WIN32)
elseif(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    # \bug Should this be set automatically?
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(CMAKE_BUILD_RPATH "@executable_path/../../../install-Debug/lib")
    elseif (CMAKE_BUILD_TYPE MATCHES "^RelWithDebInfo$")
        set(CMAKE_BUILD_RPATH "@executable_path/../../../install-RelWithDebInfo/lib")
    elseif (CMAKE_BUILD_TYPE MATCHES "^Release$")
        set(CMAKE_BUILD_RPATH "@executable_path/../../../install-Release/lib")
    endif()
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#-------------------------------------------------------------------------------
# Dependencies

find_package(Imath REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ZLIB REQUIRED)
find_package(minizip-ng REQUIRED)
find_package(OpenTimelineIO REQUIRED)
find_package(ftk REQUIRED)

if(ibis_OCIO)
    find_package(OpenColorIO REQUIRED)
    add_definitions(-DIBIS_OCIO)
endif()

if(ibis_OIIO)
    find_package(OpenImageIO)
    if(OpenImageIO_FOUND)
        add_definitions(-DIBIS_OIIO)
    endif()
endif()

if(ibis_USD)
    find_package(pxr)
    if(pxr_FOUND)
        add_definitions(-DIBIS_USD)
    endif()
endif()

if(ibis_PYTHON)
    add_definitions(-DIBIS_PYTHON)
    find_package(Python3 REQUIRED Interpreter Development)
    find_package(pybind11 REQUIRED)
endif()

#-------------------------------------------------------------------------------
# Directories

add_subdirectory(lib/ibis)
if(ibis_TESTS)
    add_subdirectory(tests)
endif()
add_subdirectory(etc/Legal)
