cmake_minimum_required(VERSION 2.6)
project(efserv)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_NAME efserv)

if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    message(STATUS "Build env: Debug")
    ADD_DEFINITIONS(-DDEBUG)
else()
    message(STATUS "Build env: Release")
    remove_definitions(-DDEBUG)
endif()

include(cmake_find_libev.cmake)
add_subdirectory(thirdparty/libeio)
add_subdirectory(thirdparty/libhttp_parser)
add_subdirectory(thirdparty/tinyformat)

include_directories(
        ${LIBEV_INCLUDE_DIR}
        ${LIBEIO_INCLUDE_DIR}
        ${LIBHTTP_PARSER_INCLUDE_DIR}
        ${LIBTINYFORMAT_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
add_dependencies(${EXECUTABLE_NAME} libeio)

target_link_libraries(
        ${EXECUTABLE_NAME}
        ${LIBEV_LIBRARY}
        ${LIBEIO_LIBRARY}
        ${LIBHTTP_PARSER_LIBRARY}
)


# add install target
SET(CMAKE_INSTALL_PREFIX /usr/local)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME} DESTINATION bin)
INSTALL(DIRECTORY tpl DESTINATION efserv)
INSTALL(DIRECTORY built-in DESTINATION efserv)
INSTALL(FILES README.md README_CN.md LICENSE .efserv_access config.ini DESTINATION efserv)


# uninstall target
include(cmake_uninstall.cmake)
add_uninstall_target()