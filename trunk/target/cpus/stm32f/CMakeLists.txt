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

add_definitions( -DSTM32F4 )

set(CMAKE_C_FLAGS "-g -ggdb -Os -mcpu=cortex-m4 -mthumb -Wextra -Wshadow -Wimplicit-function-declaration")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fno-common -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -MD -Wall -Wundef -I${KERNEL_INCLUDE}")
set(MAKE_C_FLAGS_DEBUG "-g -ggdb")

SET(sources machdep.c )

add_library(stm32f ${sources})
target_include_directories(stm32f PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
