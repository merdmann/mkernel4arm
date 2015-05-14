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
project(mkernel4arm)

SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "0")

## setup compiler environment
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DVERSION_MAJOR=${VERSION_MAJOR} -DVERSION_MINOR=${VERSION_MINOR} -DVERSION_PATCHLEVEL=${VERSION_PATCH}" )
include_directories(${PROJECT_SOURCE_DIR}/include)

## setup the build environment
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

## -----------------------------------------------------------------------------
add_subdirectory(oil)
## ------------------------------------------------------------------------------

SET(CMAKE_BUILD_TYPE "debug")


