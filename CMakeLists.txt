cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)

project(efserv)
set(EXECUTABLE_NAME efserv)

# Try find libev
# LIBEV_FOUND        - system has libev
# LIBEV_INCLUDE_DIRS - libev include directories
# LIBEV_LIBRARIES    - libraries needed to use libev
if(LIBEV_INCLUDE_DIRS AND LIBEV_LIBRARIES)
    set(LIBEV_FIND_QUIETLY TRUE)
else()
    find_path(
            LIBEV_INCLUDE_DIR
            NAMES ev.h
            HINTS ${LIBEV_ROOT_DIR}
            PATH_SUFFIXES include)
    find_library(
            LIBEV_LIBRARY
            NAME ev
            HINTS ${LIBEV_ROOT_DIR}
            PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})
    set(LIBEV_INCLUDE_DIRS ${LIBEV_INCLUDE_DIR})
    set(LIBEV_LIBRARIES ${LIBEV_LIBRARY})
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
            libev DEFAULT_MSG LIBEV_LIBRARY LIBEV_INCLUDE_DIR)
    mark_as_advanced(LIBEV_LIBRARY LIBEV_INCLUDE_DIR)
endif()
if(LIBEV_FOUND)
    message(STATUS "libev found")
else()
    message(FATAL_ERROR "libev not found")
endif()
# End of find libev



# Try find libeio
# LIBEIO_FOUND        - system has libeio
# LIBEIO_INCLUDE_DIRS - libeio include directories
# LIBEIO_LIBRARIES    - libraries needed to use libeio
if(LIBEIO_INCLUDE_DIRS AND LIBEIO_LIBRARIES)
    set(LIBEIO_FIND_QUIETLY TRUE)
else()
    find_path(
            LIBEIO_INCLUDE_DIR
            NAMES eio.h
            HINTS ${LIBEIO_ROOT_DIR}
            PATH_SUFFIXES include)
    find_library(
            LIBEIO_LIBRARY
            NAME eio
            HINTS ${LIBEIO_ROOT_DIR}
            PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})
    set(LIBEIO_INCLUDE_DIRS ${LIBEIO_INCLUDE_DIR})
    set(LIBEIO_LIBRARIES ${LIBEIO_LIBRARY})
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
            libeio DEFAULT_MSG LIBEIO_LIBRARY LIBEIO_INCLUDE_DIR)
    mark_as_advanced(LIBEIO_LIBRARY LIBEIO_INCLUDE_DIR)
endif()
if(LIBEIO_FOUND)
    message(STATUS "libeio found")
else()
    message(FATAL_ERROR "libeio not found")
endif()
# End of find libeio



# Add libhttp_parser
# LIBHTTP_PARSER_FOUND        - thirdparty has libut
# LIBHTTP_PARSER_INCLUDE_DIRS - libhttp_parser include directories
# LIBHTTP_PARSER_LIBRARIES    - libraries needed to use libhttp_parser
ADD_SUBDIRECTORY(thirdparty/libhttp_parser)
set(LIBHTTP_PARSER_INCLUDE_DIRS thirdparty/libhttp_parser)
set(LIBHTTP_PARSER_LIBRARIES http_parser)
# End of add libhttp_parser


# Add tinyformat
# TINY_FORMAT_INCLUDE_DIRS - tinyformat include directories
set(TINY_FORMAT_INCLUDE_DIRS thirdparty/tinyformat)
# End of add tinyformat


# Set include dirs
include_directories(
        ${LIBEV_INCLUDE_DIRS}
        ${LIBEIO_INCLUDE_DIRS}
        ${LIBHTTP_PARSER_INCLUDE_DIRS}
        ${TINY_FORMAT_INCLUDE_DIRS}
)
# End of set include dirs


# Sources
set(SOURCE_FILES
        main.cpp
        log.cpp
        Ini.cpp
        ServEnv.cpp
        FileHandler.cpp
        Response.cpp
        EventLoop.cpp
        StringUtils.cpp
        MimeType.cpp
        AccessRule.cpp
        UrlEncode.cpp
)
# End of sources

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(
        ${EXECUTABLE_NAME}
        ${LIBEV_LIBRARIES}
        ${LIBEIO_LIBRARIES}
        ${LIBHTTP_PARSER_LIBRARIES}
)

SET(CMAKE_INSTALL_PREFIX /usr/local)

message(STATUS "Build env: ${CMAKE_BUILD_TYPE}")

if (${CMAKE_BUILD_TYPE}  STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
endif()

INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME} DESTINATION bin)
INSTALL(DIRECTORY tpl DESTINATION efserv)
INSTALL(DIRECTORY built-in DESTINATION efserv)
INSTALL(FILES README.md README_CN.md LICENSE.md .efserv_access config.ini DESTINATION efserv)