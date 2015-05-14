/***************************************************************************
 *  $Id: pic18fdevices.h 261 2011-01-25 19:18:09Z merdmann $
 *
 *  Wed Dec  5 20:55:18 2007
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
 *  This file includes the interfaces to the hardware components which
 *  are integrated in the pic18f4550 chip.
 *
 *  \section I2C_sec I2C Interface
 *  Even thought the pic18f4550 contains an master synchronous port which
 *  can be operated in the I2C or the SPI mode we provide only an driver for
 *  the I2C interface.
 *
 *  THe I2C interface is operated in the master mode, which means the processor
 *  is the master of all connected devices.
 *
 *  The driver implementation assumes that two addresses are needed, one address
 *  (8Bit) which addresses the node on the i2c bus and a second address which is
 *  used to address internal registers of the attached hardware. This address
 *  is always send to the device first to select an internal register in the
 *  attached device.
 *
 *  \section adc_sec Analog Digital Converter
 *
 *  The Analog Digital Converter allowes access to all Analog Inputs. It
 *  runs with an aquisition time of 12 Tad and FOSC/32. The assumption is
 *  that the measurement result is return in left justified format.
 *
 *  \section cvref_sec Refernce Voltage Generator
 *
 *  If initialized, the pin A02 will be the output if the reference generator.
 *  The output range of the generator can be selected.
 *
 *  \section comp_sec Comperator
 *  If initialitzed A0/A3 and A1/A2 are used by the comperators as analog
 *  input.
 *
 * \section pwm_sec PWM
 * The pulse wide modulation module is implemented by means of T2 and
 * CPP2 which provides hit output to C1.
 *
 *
 */
#ifndef _PIC18FDEVICES_H
#define _PIC18FDEVICES_H

#include "mkernel_config.h"
#include "typedefs.h"

#if USE_I2C
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
extern BOOL _device_init_i2c_handler(void);
extern BOOL _device_intr_i2c_handler(void);
#endif


#if USE_CVREF
/** set the CVREF on A02
 * @param value a discrete value 0..15 representing a value as A02
 */
void Set_CVREF(byte value) ;

#define CVREF_24STEP 0 		/**< 24 Step voltage reference */
#define CVREF_32STEP 1		/**< 32 Step voltage reference */

/** set the range of the CVREF source.
 *  @param value - CVREF_24STEP = 0..0.667 CVsrc, CVREF_32STEP = 0.25..0.75 CVrsrc
 */
void Set_CVREF_Range(byte value);
/** driver for the cv reference: If used, the pin A02 will be used. */

extern BOOL _device_init_cvref_handler(void);
extern BOOL _device_intr_cvref_handler(void);

#endif


#if USE_ADC
/** read the value of the analog digital converter.
 * @param channel a value 0..5 to select the analog input channel from
 *                A0 .. A5.
 */
int Read_ADC( byte channel ) ;

/** device handler for the ADC */
extern BOOL _device_init_adc_handler(void);
extern BOOL _device_intr_adc_handler(void);
#endif

/** set the comperator mode.
 *  @param mode - the configuration mode as described in the manual for the
 *                processor.
 */
/** driver for the comperator */
#if USE_COMP
void Set_COMP( byte mode ) ;

/** check the status of COMP1 */
byte Comperator1(void);
/** check the status of COMP2 */
byte Comperator2(void);

extern BOOL _device_init_comp_handler(void);
extern BOOL _device_intr_comp_handler(void);

#endif


#if USE_PWM
/** configure the PWM module.
 *  @param period denotes the number of clock ticks till the CCP2 output
 *   			  is set to high.
 *  @param duty denotes the number of ticks after the CPP2 is set to
 *                low.
 *
 *  \code
 *
 *              |--- duty --->|
 *              :             :
 *              +-------------+      +-------------+      +------...
 *              |             |      |             |      |
 *        ....--+             +------+             +------+
 *              :                    :
 *              |<----- Period ----->|
 *                Clock / Prescale
 *  \endocde
 */

void PWM( unsigned period, unsigned duty );

#endif

#if USE_SPI
/**
 * Send and receive data on the SPI bus as master. Since the spi receives only
 * data when some data is transmitted, the procedure provides withe the second
 * argument a pointer where to put these octets. Is set to 0, the received data
 * will be ignored.
 *
 * @param legnth the length of the data block in octets to be send.
 * @param tx the pointer to the byte array which contains the octets to be transmitted.
 * @param rx the pointer to the array where the reived octets are to be stored.
 */
void SPI_Write( int length, POINTER tx, POINTER rx );

/* internal to OIL; should not be used directly by the application developer */
extern BOOL _device_init_spi_handler(void);
extern BOOL _device_intr_spi_handler(void);

#endif

#endif /* _PIC18FDEVICES_H */
