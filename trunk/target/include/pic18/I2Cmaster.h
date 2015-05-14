/***************************************************************************
 *  $Id: I2Cmaster.h 261 2011-01-25 19:18:09Z merdmann $
 *
 *  Wed Dec  5 07:07:57 2007
 *  Copyright  2007  Michael Erdmann
 *  Email: michael.erdmann@snafu.de
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
/** @file
 * This module contains the interface to the I2C controler of the processor. The 
 * implementation assumes, that the processor is operating a master and the devices
 * connected always requiere besides the device address an internal address to 
 * select internal resgisters for read/write. 
 */
#ifndef I2Cmaster_H
#define I2Cmaster_H

#include "typedefs.h"

/** Write a block to the given device/internal address.
 *  @param devId I2C bus address
 *  @param adr internal address of the device
 *  @param noOfBytes number of bytes to be transfered.
 *  @param ptr - pointer to the data it self.
 */
void I2CWrAdr8(  unsigned char devId, unsigned char adr, unsigned char noOfBytes, void * ptr );

/** Read a block from the given device/internal address.
 *  @param devId I2C bus address
 *  @param adr internal address of the device
 *  @param noOfBytes number of bytes to be transfered.
 *  @param ptr - pointer to the data it self.
 */
void I2CRdAdr8(  unsigned char devId, unsigned char adr, unsigned char noOfBytes, void * ptr );


/** Device descritptor for the kernel. */
extern t_dev i2c_handler;

#endif
