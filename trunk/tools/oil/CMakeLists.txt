##
## Copyright (C) 2014-2015 Michael Erdmann <michael.erdmann@snafu.de>
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
##  Created on: April 2, 2014
##      Author: Michael Erdmann
## 
## ---------------------------------------------------------------------------

SET(CMAKE_BUILD_TYPE "debug")

## contents of the core library
SET(source attributes.c  bucket.c  objects.c  oil.c  parser.c  scanner.c  sysgen.c)

## the binary of the game
add_executable(oil oil.c ${source})
