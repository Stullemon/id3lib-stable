// $Id: version.cpp,v 1.3 2000/01/04 15:42:49 eldamitri Exp $
// 
// This program is free software; you can distribute it and/or modify it under
// the terms discussed in the COPYING file, which should have been included
// with this distribution.
//  
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING file for more details.
//  
// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where
// to send such submissions.

#include "version.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

const char *ID3_Name()
{
  return PACKAGE;
}

const char *ID3_Release()
{
  return VERSION;
}

const char *ID3_FullName()
{
  return ID3LIB_FULLNAME;
}

unsigned short ID3_Version()
{
  return ID3LIB_VERSION;
}

unsigned short ID3_Revision()
{
  return ID3LIB_REVISION;
}

unsigned short ID3_Patch()
{
  return ID3LIB_PATCH;
}

// $Log: version.cpp,v $
// Revision 1.3  2000/01/04 15:42:49  eldamitri
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
// Revision 1.2  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.1  1999/11/30 20:19:26  scott
// Added.  Implements version functions declared in version.h.
//
