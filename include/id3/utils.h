/* $Id: utils.h,v 1.1 2000/04/20 00:40:31 eldamitri Exp $

 * id3lib: a software library for creating and manipulating id3v1/v2 tags
 * Copyright 1999, 2000  Scott Thomas Haug
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.

 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 * The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/
 */


#ifndef __ID3LIB_TYPES_H__
#define __ID3LIB_TYPES_H__

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#else
# define false (0)
# define true (!false)
  typedef int bool;
#endif /* __cplusplus */

#define BS_SIZE (sizeof(luint)*8)
#define BS_SET(v,x)   ((v)[(x) / BS_SIZE] |=  (1 << ((x) % BS_SIZE)))
#define BS_CLEAR(v,x) ((v)[(x) / BS_SIZE] &= ~(1 << ((x) % BS_SIZE)))
#define BS_ISSET(v,x) ((v)[(x) / BS_SIZE] &   (1 << ((x) % BS_SIZE)))

  /*
   * The following is borrowed from glib.h (http://www.gtk.org)
   */
#ifdef NATIVE_WIN32

  /* On native Win32, directory separator is the backslash, and search path
   * separator is the semicolon.
   */
#  define ID3_DIR_SEPARATOR '\\'
#  define ID3_DIR_SEPARATOR_S "\\"
#  define ID3_SEARCHPATH_SEPARATOR ';'
#  define ID3_SEARCHPATH_SEPARATOR_S ";"

#else  /* !NATIVE_WIN32 */

#  ifndef __EMX__
  /* Unix */

#    define ID3_DIR_SEPARATOR '/'
#    define ID3_DIR_SEPARATOR_S "/"
#    define ID3_SEARCHPATH_SEPARATOR ':'
#    define ID3_SEARCHPATH_SEPARATOR_S ":"

#  else
  /* EMX/OS2 */

#    define ID3_DIR_SEPARATOR '/'
#    define ID3_DIR_SEPARATOR_S "/"
#    define ID3_SEARCHPATH_SEPARATOR ';'
#    define ID3_SEARCHPATH_SEPARATOR_S ";"

#  endif

#endif /* !NATIVE_WIN32 */



#ifndef NULL
#define NULL ((void*) 0)
#endif

#undef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
  
#undef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
  
#undefABS
#define ABS(a)   (((a) < 0) ? -(a) : (a))

#undef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#if defined __cplusplus
}
#endif

#endif
