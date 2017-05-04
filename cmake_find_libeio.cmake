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