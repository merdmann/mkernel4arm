##
## Copyright (C) 2014 Michael Erdmann <michael.erdmann@snafu.de>
##
## Makefile is part of mkernel4arm.
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
CPU=stm32f
toolchain=arm-none-eabi
board=disco
app=myapp

root=$(shell pwd)

target=../trunk/cpus/$(CPU)
toolchain_def=$(root)/trunk/toolchains/$(toolchain).txt

all clean :: build/tools build/myapp
	$(MAKE) -C./build/tools $@
	$(MAKE) -C./build/myapp $@

build/tools :
	mkdir -p build/tools
	( cd build/tools && cmake -G "Unix Makefiles"  $(root)/trunk/tools )

build/myapp :
	mkdir -p build/myapp
	( cd  build/myapp && cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$(toolchain_def) $(root)/trunk/target )

clean ::
	rm -rf build
