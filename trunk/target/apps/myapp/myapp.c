/***************************************************************************
 *  $Id: myapp.c 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Fri Nov 23 06:52:21 2007
 *  Copyright  2007  Michael Erdmann
 *  michael.erdmann@snafu.de
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "typedefs.h"
#include "kernel.h"
#include "alarm.h"
#include "event.h"

#include <libopencm3/stm32/gpio.h>

#include "myapp_defs.h"


TASK(ToggelLED) {
    gpio_toggle(GPIOG, GPIO14);
}


void StartupHook() {
 
}


int main(int argc, char* argv[]) {
    StartOS(Application_Mode_stm234f);
    return 0;
}
