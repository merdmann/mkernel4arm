 /**************************************************************************
 *  $Id: mkernel_config.h 262 2011-01-29 19:07:36Z merdmann $
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

#ifndef _MKERNEL_CONFIG_H
#define _MKERNEL_CONFIG_H

/** @file
 *  Kernel Configuration Parameters
 *
 * \section overview_sec Overview
 * This configuration file is used to configure the kernel. After modifying
 * any configuration parameter the complete os has to be rebuild, which means
 * after changing the parameters execute the following commands:
 *
 * \code
 *  $ make clean
 *  $ make deps
 *  $ make
 * \endcode
 *
 *
 * \section memory_sec Memory Layout
 * The basic memory layout is shown below. The linker control file assumes
 * that the address 0x0000 - 0x0100 is used for the access bank. Starting
 * from this address on, the linker will place global variables.
 *
 * Data of drivers is not kept there.
 *
 * \code
 *			+------------------+ 0x0000
 *			|XXXXXXXXXXXXXXXXXX|          Reserver for compiler
 *			|XXXXXXXXXXXXXXXXXX|
 *			+------------------+ 0x0100   start of data
 *			|                  |          global and static variables will be stored
 *			|                  |          here.
 *			+------------------+ 0x0200
 *			|                  |
 *			|                  |
 *			+------------------+ 0x0300
 *			|                  |
 *			|                  |
 *			+------------------+ 0x0400
 *			|                  |
 *			|                  |
 *			+------------------+ 0x0500
 *			|                  |
 *			|                  |
 *			+------------------+ 0x0600
 *			|                  |
 *			|                  |
 *			+------------------+ 0x0700   kdata
 *			|                  |
 *			|                  |
 *			+------------------+ 0x07ff
 *
 * \endcode
 *
 */

/***************************************************************************************/
/*                    C O N F I G U R A T I O N    S E C T I O N                       */
/***************************************************************************************/

/*************************************************************************************/
/*                D E R I V E D   C O N F I G U R A T I O N  D A T A                 */
/*************************************************************************************/


/** use the watchdog timer.
 *  if this is set to 1, the watchdog timer of the CPU is used. The idle task
 *  clears the watchdog timer. If the idle task stops running for a some reason
 *  for a while, the processor is restarted and a the so called process alarm
 *  indication is set (see trace.h).
 */
#define USE_WATCHDOG  0


/** ticks intervall in ms.
 *  if you want to change this value please be aware that there are some
 *  limits to this value. If it is selected to small the CPU will be busy
 *  handle the scheduling.
 *  Please note, if using T2 to generate the tick clock, the register PR2
 *  is an 8 Bit register. Therefore we do simulate a 16 bit counter in software.
 *  In case of T0 all this is done in hardware.
 *
 *  The timer is selected by changing the USE_TICK_TIMER value in the
 *  file make.conf manually.
 */

#define USE_I2C		1	/**< Include the I2C */
#define USE_ADC		1   /**< Include the analog digital converter */
#define USE_CVREF 	1	/**< Include the cvref module */
#define USE_COMP 	1	/**< include the comperator module */
#define USE_USB		0	/**< include the usb interface */
#define USE_SPI		1	/**< Include the SPI module */

/** use the PWM module. The output will be present on C1. If this mode
 *  is selected it is not possible to address the LED's and the relays
 *  any more.
 */
#define USE_PWM  	0

/** use the LED on the SBC44UC board to blink according to the kernel ticks.*/
#define USE_TICK_INDICATOR 1

#endif /* _MKERNEL_CONFIG_H */

