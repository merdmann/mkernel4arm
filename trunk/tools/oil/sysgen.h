/***************************************************************************
 *  $Id$
 *
 *  Aug 31, 2008 9:36:08 AM
 *  Copyright (C) 2008 Michael Erdmann
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

 /**
  * This is the system generator
  *
  */

#ifndef _SYSGEN_H_
#define _SYSGEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bucket.h"
#include "objects.h"


int sysgen_generate(char *target, char *name);

#endif
