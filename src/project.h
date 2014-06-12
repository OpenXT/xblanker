/*
 * project.h:
 *
 * Copyright (c) 2009 James McKenzie <20@madingley.org>,
 * All rights reserved.
 *
 */

/*
 * Copyright (c) 2013 Citrix Systems, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*
 * $Id: project.h,v 1.2 2009/07/31 12:59:40 jamesmck Exp $
 */

/*
 * $Log: project.h,v $
 * Revision 1.2  2009/07/31 12:59:40  jamesmck
 * *** empty log message ***
 *
 * Revision 1.1  2009/07/31 11:58:28  jamesmck
 * *** empty log message ***
 *
 */

#ifndef __PROJECT_H__
#define __PROJECT_H__


#include "config.h"

#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#endif
#include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#elif defined(HAVE_SYS_INT_TYPES_H)
#include <sys/int_types.h>
#endif

#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/select.h>
#include <xenstore.h>

#include <pciaccess.h>

#include "prototypes.h"

#endif /* __PROJECT_H__ */
