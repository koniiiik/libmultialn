CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(libmultialn)

OPTION(ENABLE_TEST "Enable unit tests" OFF)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra --std=c++0x")
ENDIF()

# We need to extend CMAKE_MODULE_PATH to our PROJECT_SOURCE_DIR since we
# hold the FindLibCDS.cmake file and the project itself doesn't ship
# LibCDSConfig.cmake.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

FIND_PACKAGE(LibCDS REQUIRED)

INCLUDE_DIRECTORIES(${LIBCDS_INCLUDE_DIRS})

ADD_SUBDIRECTORY(src)
IF(ENABLE_TEST)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(lib/gtest-1.6.0)
    ADD_SUBDIRECTORY(test)
ENDIF(ENABLE_TEST)
ADD_SUBDIRECTORY(examples)
