cmake_minimum_required(VERSION 3.0)
project(disco C)

set(KERNEL_INCLUDE ${PROJECT_SOURCE_DIR}/../include)
include_directories(${KERNEL_INCLUDE} ${OPENLIBCM3}/include ./ ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DSTM32F4 )

SET(sources console.c  font-7x12.c  gfx.c  lcd-spi.c  sdram.c)

add_library(disco ${sources})
target_include_directories(disco PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
