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
project(stm32f C)

####
set(KERNEL_INCLUDE ../../include)
include_directories(${KERNEL_INCLUDE} ${OPENLIBCM3_INCLUDE} ./ ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DSTM32F4 )

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -MD -Wall -Wundef -I${KERNEL_INCLUDE}")
set(MAKE_C_FLAGS_DEBUG "-g -ggdb")

set( CMAKE_EXE_LINKER_FLAGS "--static -nostartfiles")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${OPENLIBCM3_LIB}")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${PROJECT_SOURCE_DIR}/stm32f429i-discovery.ld")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=myapp.map")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-gc-sections")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group")

##
## generate the system from the oil spec.
##
add_custom_command(
	OUTPUT myapp_init.c 
	MAIN_DEPENDENCY myapp.oil 
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${OIL} -I${KERNEL_INCLUDE} -a stm234f myapp.oil -o ${CMAKE_CURRENT_BINARY_DIR}/myapp
) 

##
## Build the executable
##
add_executable( myapp.elf myapp.c myapp_init.c)
target_link_libraries( myapp.elf kernel stm32f disco libopencm3_stm32f4.a )

##
## Flash the board
##
add_custom_command(
	TARGET myapp.elf POST_BUILD
	MAIN_DEPENDENCY myapp.elf
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND arm-eabi-size ${EXECUTABLE_OUTPUT_PATH}/myapp.elf	
	COMMAND ${OOCD} ${OOCD_ARGS}
) 

##
## Run the debugger 
##
add_custom_target( debug  
	DEPENDS myapp.elf
	WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    COMMAND ${OOCD} --file interface/stlink-v2.cfg -c "gdb_memory_map enable" -c "gdb_flash_program enable" --file target/stm32f4x.cfg &
    COMMAND adg myapp.elf
) 
