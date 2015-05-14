# ***************************************************************************
# *  $Id: Makefile 265 2011-02-04 16:32:11Z merdmann $
# *
# *  Thu Nov 29 21:14:09 2007
# *  Copyright  2007, 2008 Michael Erdmann
# *  Email: michael.erdmann@snafu.de
# ****************************************************************************/

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
sinclude make.conf
include $(ROOT)/make.conf

## name of the application

app=myapp
mode=IOR5E

## .................  DO NOT MODIFY FROM HERE ON ...........................
src=$(app).c
hex=$(app).hex
objs=$(app).o

init=$(app)_init
defs=$(app)_defs.h

all: tools $(hex)

tools:
	(cd .. && make )

$(init).o: $(init).c 

$(init).c $(defs): $(app).oil
	$(OIL) -I../../include -q -a $(mode) $(app).oil -o $(app)

$(app).o: $(app).c  $(defs)

$(hex): $(objs) $(LIBS) $(init).o
	gplink $(LDFLAGS) $(ROOT)/kernel/main.lkr -o $@ $(objs) $(init).o $(LIBS)
	@echo
	@awk -f $(MEM) ../../kernel/main.lkr $(subst .hex,.map,$@)
	@echo

load: $(hex)
	sudo $(BINDIR)/fsusb $(hex)

doc ::

deps:
	makedepend -- $(CFLAGS) $(src)
 
clean: 
	$(RM) *.cod *.hex *.lst *.rel *.sym *.o *.asm *.map *~ *.lib *.bak
	$(RM) $(app)_init.* $(app)_defs.h

# DO NOT DELETE
