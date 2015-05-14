##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

.PHONY: build

PREFIX		?= arm-none-eabi

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

LDSCRIPT := ./stm32f429i-discovery.ld
LDLIBS := -lm
OPENCM3_DIR := ../libopencm3
BINARY := lcd-serial
OBJS := clock.o  console.o  font-7x12.o  gfx.o  lcd-spi.o  sdram.o

include ./mk/Makefile.include

#####################
### default target ##
#####################
all : $(BINARY).elf 
	arm-none-eabi-size --format=berkeley $(BINARY).elf

clean:
	@#printf "  CLEAN\n"
	$(Q)$(RM) *.o *.d *.elf *.bin *.hex *.srec *.list *.map
