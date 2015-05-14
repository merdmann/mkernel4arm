/***************************************************************************
 *  $Id: usb.h 150 2008-06-16 20:17:25Z merdmann $
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
 * This file contains all USB hardware specific definitions which are
 * not available in the standard header files.
 */

#include "typedefs.h"

#ifndef _USB_H
#define _USB_H


/** The setup package (DATA0) send by the host via the endpoint 0 */
typedef union {
    struct {
        byte bmRequestType;
        byte bRequest; 
        word wValue;
        word wIndex;
        word wLength;
    };
    struct {
        unsigned :8;
        unsigned :8;
        WORD W_Value;
        WORD W_Index;
        WORD W_Length;
    };
    struct {
        unsigned Recipient:5;           //Device,Interface,Endpoint,Other
        unsigned RequestType:2;         //Standard,Class,Vendor,Reserved
        unsigned DataDir:1;             //Host-to-device,Device-to-host

        unsigned :8;
        byte bFeature;                  //DEVICE_REMOTE_WAKEUP,ENDPOINT_HALT
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
    };
    struct {
        unsigned :8;
        unsigned :8;
        byte bDscIndex;                 //For Configuration and String DSC Only
        byte bDscType;                  //Device,Configuration,String
        word wLangID;                   //Language ID
        unsigned :8;
        unsigned :8;
    };
    struct {
        unsigned :8;
        unsigned :8;
        byte bDevADR;                   //Device Address 0-127
        byte bDevADRH;                  //Must equal zero
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
    };
    struct {
        unsigned :8;
        unsigned :8;
        byte bCfgValue;                 //Configuration Value 0-255
        byte bCfgRSD;                   //Must equal zero (Reserved)
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
    };
    struct {
        unsigned :8;
        unsigned :8;
        byte bAltID;                    //Alternate Setting Value 0-255
        byte bAltID_H;                  //Must equal zero
        byte bIntfID;                   //Interface Number Value 0-255
        byte bIntfID_H;                 //Must equal zero
        unsigned :8;
        unsigned :8;
    };
    struct {
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
        byte bEPID;                     //Endpoint ID (Number & Direction)
        byte bEPID_H;                   //Must equal zero
        unsigned :8;
        unsigned :8;
    };
    struct{
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned EPNum:4;               //Endpoint Number 0-15
        unsigned :3;
        unsigned EPDir:1;               //Endpoint Direction: 0-OUT, 1-IN
        unsigned :8;
        unsigned :8;
        unsigned :8;
    };
} t_setup;


#define _BSTALL     0x04                //Buffer Stall enable
#define _DTSEN      0x08                //Data Toggle Synch enable
#define _INCDIS     0x10                //Address increment disable
#define _KEN        0x20                //SIE keeps buff descriptors enable
#define _DAT0       0x00                //DATA0 packet expected next
#define _DAT1       0x40                //DATA1 packet expected next
#define _DTSMASK    0x40                //DTS Mask
#define _USIE       0x80                //SIE owns buffer
#define _UCPU       0x00                //CPU owns buffer


/* Endpoint parameters */
#define EP_CTRL     0x06            // Cfg Control pipe for this ep
#define EP_OUT      0x0C            // Cfg OUT only pipe for this ep
#define EP_IN       0x0A            // Cfg IN only pipe for this ep
#define EP_OUT_IN   0x0E            // Cfg both OUT & IN pipes for this ep
#define HSHK_EN     0x10            // Enable handshake packet
                                    // Handshake should be disable for isoch

/* UCFG Parameters */
#define _PPBM0      0x00            // Pingpong Buffer Mode 0
#define _PPBM1      0x01            // Pingpong Buffer Mode 1
#define _PPBM2      0x02            // Pingpong Buffer Mode 2
#define _LS         0x00            // Use Low-Speed USB Mode
#define _FS         0x04            // Use Full-Speed USB Mode
#define _TRINT      0x00            // Use internal transceiver
#define _TREXT      0x08            // Use external transceiver
#define _PUEN       0x10            // Use internal pull-up resistor
#define _OEMON      0x40            // Use SIE output indicator
#define _UTEYE      0x80            // Use Eye-Pattern test

#define OUT         0
#define IN          1

#define SETUP_TOKEN         0x0d          //0b00001101
#define OUT_TOKEN           0x01          //0b00000001
#define IN_TOKEN            0x09          //0b00001001

#define GET_STATUS  0
#define CLR_FEATURE 1
#define SET_FEATURE 3
#define SET_ADR     5
#define GET_DSC     6
#define SET_DSC     7
#define GET_CFG     8
#define SET_CFG     9
#define GET_INTF    10
#define SET_INTF    11
#define SYNCH_FRAME 12

/* bmRequestType Definitions */
#define HOST_TO_DEVICE         0
#define DEVICE_TO_HOST         1

#define STANDARD            0x00
#define CLASS               0x01
#define VENDOR              0x02

#define RCPT_DEV            0
#define RCPT_INTF           1
#define RCPT_EP             2
#define RCPT_OTH            3

/* Descriptor Types */
#define DSC_DEV     0x01
#define DSC_CFG     0x02
#define DSC_STR     0x03
#define DSC_INTF    0x04
#define DSC_EP      0x05


#define _EP01_OUT   0x01
#define _EP01_IN    0x81
#define _EP02_OUT   0x02
#define _EP02_IN    0x82
#define _EP03_OUT   0x03
#define _EP03_IN    0x83
#define _EP04_OUT   0x04
#define _EP04_IN    0x84
#define _EP05_OUT   0x05
#define _EP05_IN    0x85
#define _EP06_OUT   0x06
#define _EP06_IN    0x86
#define _EP07_OUT   0x07
#define _EP07_IN    0x87
#define _EP08_OUT   0x08
#define _EP08_IN    0x88
#define _EP09_OUT   0x09
#define _EP09_IN    0x89
#define _EP10_OUT   0x0A
#define _EP10_IN    0x8A
#define _EP11_OUT   0x0B
#define _EP11_IN    0x8B
#define _EP12_OUT   0x0C
#define _EP12_IN    0x8C
#define _EP13_OUT   0x0D
#define _EP13_IN    0x8D
#define _EP14_OUT   0x0E
#define _EP14_IN    0x8E
#define _EP15_OUT   0x0F
#define _EP15_IN    0x8F


/* USB 2.0 Table 8.9 */
typedef struct {
    byte bLength;       byte bDescriptorType;		word bcdUSB;
    byte bDeviceCls;    byte bDeviceSubCls;			byte bDeviceProtocol;
    byte bMaxPktSize0;  word idVendor;      		word idProduct;
    word bcdDevice;     byte iManufacturer;         byte iProduct;
    byte iSerialNum;    byte bNumCconfigurations;
} t_usb_dev_dsc;

/******************************************************************************
 * USB Configuration Descriptor Structure
 *****************************************************************************/
typedef struct {
    byte bLength;       byte bDscType;      word wTotalLength;
    byte bNumIntf;      byte bCfgValue;     byte iCfg;
    byte bmAttributes;  byte bMaxPower;
} t_usb_cfg_dsc;

/******************************************************************************
 * USB Interface Descriptor Structure
 *****************************************************************************/
typedef struct _USB_INTF_DSC
{
    byte bLength;       byte bDscType;      byte bIntfNum;
    byte bAltSetting;   byte bNumEPs;       byte bIntfCls;
    byte bIntfSubCls;   byte bIntfProtocol; byte iIntf;
} USB_INTF_DSC;

/******************************************************************************
 * USB Endpoint Descriptor Structure
 *****************************************************************************/
typedef struct _USB_EP_DSC
{
    byte bLength;       byte bDscType;      byte bEPAdr;
    byte bmAttributes;  word wMaxPktSize;   byte bInterval;
} USB_EP_DSC;

/* Configuration Attributes */
#define _DEFAULT    0x01<<7         //Default Value (Bit 7 is set)
#define _SELF       0x01<<6         //Self-powered (Supports if set)
#define _RWU        0x01<<5         //Remote Wakeup (Supports if set)

/* Endpoint Transfer Type */
#define _CTRL       0x00            //Control Transfer
#define _ISO        0x01            //Isochronous Transfer
#define _BULK       0x02            //Bulk Transfer
#define _INT        0x03            //Interrupt Transfer

/* Isochronous Endpoint Synchronization Type */
#define _NS         0x00<<2         //No Synchronization
#define _AS         0x01<<2         //Asynchronous
#define _AD         0x02<<2         //Adaptive
#define _SY         0x03<<2         //Synchronous

/* Isochronous Endpoint Usage Type */
#define _DE         0x00<<4         //Data endpoint
#define _FE         0x01<<4         //Feedback endpoint
#define _IE         0x02<<4         //Implicit feedback Data endpoint

#define OUT         0
#define IN          1

#define PIC_EP_NUM_MASK 0b01111000
#define PIC_EP_DIR_MASK 0b00000100

#define EP00_OUT    (0x00<<3)|(OUT<<2)
#define EP00_IN     (0x00<<3)|(IN<<2)
#define EP01_OUT    (0x01<<3)|(OUT<<2)
#define EP01_IN     (0x01<<3)|(IN<<2)
#define EP02_OUT    (0x02<<3)|(OUT<<2)
#define EP02_IN     (0x02<<3)|(IN<<2)
#define EP03_OUT    (0x03<<3)|(OUT<<2)
#define EP03_IN     (0x03<<3)|(IN<<2)
#define EP04_OUT    (0x04<<3)|(OUT<<2)
#define EP04_IN     (0x04<<3)|(IN<<2)
#define EP05_OUT    (0x05<<3)|(OUT<<2)
#define EP05_IN     (0x05<<3)|(IN<<2)
#define EP06_OUT    (0x06<<3)|(OUT<<2)
#define EP06_IN     (0x06<<3)|(IN<<2)
#define EP07_OUT    (0x07<<3)|(OUT<<2)
#define EP07_IN     (0x07<<3)|(IN<<2)
#define EP08_OUT    (0x08<<3)|(OUT<<2)
#define EP08_IN     (0x08<<3)|(IN<<2)
#define EP09_OUT    (0x09<<3)|(OUT<<2)
#define EP09_IN     (0x09<<3)|(IN<<2)
#define EP10_OUT    (0x0A<<3)|(OUT<<2)
#define EP10_IN     (0x0A<<3)|(IN<<2)
#define EP11_OUT    (0x0B<<3)|(OUT<<2)
#define EP11_IN     (0x0B<<3)|(IN<<2)
#define EP12_OUT    (0x0C<<3)|(OUT<<2)
#define EP12_IN     (0x0C<<3)|(IN<<2)
#define EP13_OUT    (0x0D<<3)|(OUT<<2)
#define EP13_IN     (0x0D<<3)|(IN<<2)
#define EP14_OUT    (0x0E<<3)|(OUT<<2)
#define EP14_IN     (0x0E<<3)|(IN<<2)
#define EP15_OUT    (0x0F<<3)|(OUT<<2)
#define EP15_IN     (0x0F<<3)|(IN<<2)


#endif /* _USB_H */
