/***************************************************************************
 *  $Id: resource.h 271 2011-02-20 10:31:23Z merdmann $
 *
 *  Mon Nov 26 06:50:00 2007
 *  Copyright  2007 Michael Erdmann
 *  Email:Michael.Erdmann@snafu.de
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
 *  OSEK/VDX Handle Resources
 *  This file contains the code to handle resources according to the
 *  OSEK/VDX standard Section 13.4.
 *
 */
#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "typedefs.h"

/** A resource instance is used to realize resources, but it should
 *  not be used directly, only the reference to the instances is used.
 *
 *  An instance consists of an owner id which is the process id of the
 *  owner and a so called reference. The reference handle my be used
 *  to point to additional data.
 */

/** this is the default value of the resource lock */
#define NO_OWNER  0xff

typedef struct {
	t_task_id owner;		/* the owner of the Resource */
	POINTER reference;		/* the handle to the resource it self */
} ResourceInstance;

typedef DATA ResourceInstance* ResourceType;

#define InitResource(x) {(x).owner = NO_OWNER; }

#define DeclareResource(x) extern ResourceInstance x

/** GetResource get a critical section
 *  This call serves to enter critical sections in the code that are
 *  assigned to the resource referenced by <ResID>. A critical
 *  section shall always be left using ReleaseResource.
 *
 *  @param ResID - Reference to resource
 */
StatusType GetResource( ResourceType ResID );

/**
 * ReleaseResource is the counterpart of GetResource and
 * serves to leave critical sections in the code that are assigned to
 * the resource referenced by <ResID>.
 *
 * @param ResID - Reference to the resource,
 */
StatusType ReleaseResource( ResourceType ResID );

#define ResourceOwner(x) (x).owner

#endif

