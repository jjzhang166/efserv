cmake_minimum_required(VERSION 2.6)
project(libeio)

include(ExternalProject)
ExternalProject_Add(
        libeio
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && ./autogen.sh && ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        BUILD_IN_SOURCE 1
)

set(LIBEIO_INCLUDE_DIR  ${CMAKE_CURRENT_BINARY_DIR}/include             PARENT_SCOPE)
set(LIBEIO_LIBRARY      ${CMAKE_CURRENT_BINARY_DIR}/lib/libeio.a        PARENT_SCOPE)