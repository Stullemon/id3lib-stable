// $Id: types.h,v 1.5 2000/04/05 05:20:52 eldamitri Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#ifndef ID3LIB_TYPES_H
#define ID3LIB_TYPES_H

#include "globals.h"

#ifdef __DLL
#define DLLEXPORT       __declspec ( dllexport )
#define CDLLEXPORT      extern "C" __declspec ( dllexport )
#else
#define DLLEXPORT
#define CDLLEXPORT
#endif

#define BS_SIZE (sizeof(luint)*8)
#define BS_SET(v,x)   ((v)[(x) / BS_SIZE] |=  (1 << ((x) % BS_SIZE)))
#define BS_CLEAR(v,x) ((v)[(x) / BS_SIZE] &= ~(1 << ((x) % BS_SIZE)))
#define BS_ISSET(v,x) ((v)[(x) / BS_SIZE] &   (1 << ((x) % BS_SIZE)))

#ifndef NULL
#define NULL (0L)
#endif

#ifndef MIN
inline lsint MIN(lsint x, lsint y)
{
  return x < y ? x : y;
}
#endif

#ifndef MAX
inline lsint MAX(lsint x, lsint y)
{
  return x > y ? x : y;
}
#endif

#endif

// $Log: types.h,v $
// Revision 1.5  2000/04/05 05:20:52  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.4  2000/01/04 15:42:22  eldamitri
// * include/id3/field.h:
// * include/id3/int28.h:
// * include/id3/misc_support.h:
// * include/id3/tag.h:
// * include/id3/types.h:
// * src/id3/dll_wrapper.cpp
// * src/id3/error.cpp
// * src/id3/field.cpp
// * src/id3/field_binary.cpp
// * src/id3/field_integer.cpp
// * src/id3/field_string_ascii.cpp
// * src/id3/field_string_unicode.cpp
// * src/id3/frame.cpp
// * src/id3/frame_parse.cpp
// * src/id3/frame_render.cpp
// * src/id3/header.cpp
// * src/id3/header_frame.cpp
// * src/id3/header_tag.cpp
// * src/id3/int28.cpp
// * src/id3/misc_support.cpp
// * src/id3/tag.cpp
// * src/id3/tag_file.cpp:
// * src/id3/tag_find.cpp:
// * src/id3/tag_parse.cpp:
// * src/id3/tag_parse_lyrics3.cpp:
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.3  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:11  scott
// moved from src/id3 to include/id3
//
// Revision 1.7  1999/11/30 20:14:22  scott
// Removed include for wchar.h
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/25 19:24:24  scott
// Removed typedefs, which are now included in the globas.h file.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
