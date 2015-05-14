/***************************************************************************
 *  $Id: usbdriver.h 265 2011-02-04 16:32:11Z merdmann $
 *
 *  Thu Nov 29 20:55:57 2007
 *  Copyright  2007 Michael Erdmann
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
 *
 * This file contains the interfaces to the usb driver. The usb driver
 * provides access the the functionality of sending/receiving packages.
 * All other things will be implemented on a higher level.
 *
 * \section sec Usage
 * The code fragment below provides a package for sending.
 * \code
 *
 * t_epdata *ep ;
 *
 * USB_Activate();
 *
 * ep = USB_Allocate_EP(0);
 *
 * USB_Send(ep, buffer, length, DATA1 );
 *
 *
 * \endcode
 *
 */
#include "kernel.h"
#include "usb.h"

#ifndef _USBDRIVER_H
#define _USBDRIVER_H

typedef byte t_ep;
typedef byte t_bid;

/* data stored per end point */
typedef __near struct {
	t_ep	 ep;
	POINTER  buffer;
	int		 bytes_to_send;		/* number of bytes still to send */
	unsigned length;			/* number of bytes recevied with PID*/
	byte     PID;				/* USB PID */
	ResourceInstance done;
} t_epdata;

#define MAX_EP		2

/** Start the USB interface. */
void USB_Activate(void);

/** shutdown the interface. */
void USB_Shutdown(void);

/* specify the next data packaet type */
typedef enum {
	DATA0,
	DATA1,
	DATAX
} t_dts;

/**
 * Wait for an transmission from the host
 *
 * @param p pointer to the endpoint data
 * @param next indicates whether a data0,data1 token is to be used.
 */
StatusType USB_Wait( t_epdata *p, t_dts next);

/**
 * Provide an package which is to be retrieved by the host for the given
 * endpoint.
 *
 * Please note, that send will not wait until the transaction has been
 * executed, but it will block the next send packet or USB_Wait until
 * the transmission has been completed.
 *
 * @param p reference to the end point data
 * @param buffer references to the packet
 * @param length length of the packaet
 * @param first indicates whether a data0,data1 token is to be used.
 */
StatusType USB_Send(t_epdata *p, void *buffer, unsigned length, t_dts first );

/**
 * Allocate an end point.
 *
 * @param ep number of the end point.
 */
t_epdata *USB_Allocate_EP(t_ep ep);

#define NBR_BUFFERS 			2
#define EP0_BUFF_SIZE           64   // 8, 16, 32, or 64

extern unsigned _usb_packet_tx;
extern unsigned _usb_packet_rx;
#endif
