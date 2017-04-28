cmake_minimum_required(VERSION 2.6)
project(libeio)

include(ExternalProject)
ExternalProject_Add(
        eio_imported
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && ./autogen.sh && ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}
        BUILD_IN_SOURCE 1
)

add_library(eio UNKNOWN IMPORTED)
set_target_properties(eio PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib/libeio.a)

add_dependencies(eio eio_imported)