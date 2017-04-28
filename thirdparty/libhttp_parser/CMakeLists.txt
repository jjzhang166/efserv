cmake_minimum_required(VERSION 2.6)
project(libhttp_parser)

add_library(http_parser STATIC http_parser.c)

set(LIBHTTP_PARSER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBHTTP_PARSER_LIBRARY http_parser PARENT_SCOPE)