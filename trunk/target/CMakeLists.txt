##
## Copyright (C) 2014 Michael Erdmann <michael.erdmann@snafu.de>
##
## CMakeList.txt is part of mkernel4arm.
##
## mkernel4arm is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## mkernel4arm is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with muon_reloaded. If not, see <http://www.gnu.org/licenses/>.
##
##  Created on: April 2, 2015
##      Author: Michael Erdmann
## 
cmake_minimum_required(VERSION 3.0)


## where is libopencm3
set(OPENLIBCM3  /c/lib/libopencm3/arm-none-eabi )

## open ocd
set(OOCD /c/tools/openocd-0.9.0/bin-x64/openocd.exe)
set(OOCD_INTERFACE stlink-v2)
set(OOCD_BOARD stm32f4discovery)

set(OOCD_ARGS "${OOCD_ARGS} -f interface/${OOCD_INTERFACE}.cfg")
set(OOCD_ARGS "${OOCD_ARGS} -f board/${OOCD_BOARD}.cfg")
##set(OOCD_ARGS "${OOCD_ARGS} -c ft2232_serial ${OOCD_SERIAL}" )
set(OOCD_ARGS "${OOCD_ARGS} -c init")
set(OOCD_ARGS "${OOCD_ARGS} -c \"reset init\"")
set(OOCD_ARGS "${OOCD_ARGS} -c \"flash write_image erase ../../bin/myapp.elf\"")
set(OOCD_ARGS "${OOCD_ARGS} -c reset")
set(OOCD_ARGS "${OOCD_ARGS} -c shutdown")


## this is the stuff a specfied in: 
## JPL Institutional Coding Standard for the C Programming Language

##set(JPL "-Wtraditional -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion")

set(CMAKE_C_FLAGS "-g -ggdb -Os -mcpu=cortex-m4 -mthumb -Wall -Winline -pedantic -std=iso9899:1999 ")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${JPL}")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fno-common -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(MAKE_C_FLAGS_DEBUG "-g -ggdb")

## setup the build environment
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

SET(OIL ${PROJECT_BINARY_DIR}/../bin/oil)

## -----------------------------------------------------------------------------
add_subdirectory(kernel)
add_subdirectory(cpus)
add_subdirectory(boards)
add_subdirectory(apps)
## ------------------------------------------------------------------------------

SET(CMAKE_BUILD_TYPE "debug")
