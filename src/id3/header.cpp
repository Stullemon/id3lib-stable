// $Id: header.cpp,v 1.11 2000/01/04 15:42:49 eldamitri Exp $
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

#include <cstring>
#include <memory.h>
#include "header.h"
#include "error.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_HeaderInfo ID3_VersionInfo[] =
  {
    //         SIZEOF SIZEOF SIZEOF EXT    EXT  EXPERIM
    // VER REV FRID   FRSZ   FRFL   HEADER SIZE BIT
    {  2,  0,  3,     3,     0,     false, 0,   false },
    {  2,  1,  3,     3,     0,     true,  8,   true },
    {  3,  0,  4,     4,     2,     false, 10,  false },
    {  0 }
  };
  
  
ID3_HeaderInfo *ID3_LookupHeaderInfo(uchar ver, uchar rev)
{
  ID3_HeaderInfo *info = NULL;
  for (size_t i = 0; ID3_VersionInfo[i].ucVersion != 0; i++)
    if (ID3_VersionInfo[i].ucVersion  == ver &&
        ID3_VersionInfo[i].ucRevision == rev)
    {
      info = &ID3_VersionInfo[i];
      break;
    }
    
  return info;
}

ID3_Header::ID3_Header(void)
{
  SetVersion(ID3v2_VERSION, ID3v2_REVISION);
  __ulDataSize = 0;
  __ulFlags = 0;
}

void ID3_Header::SetVersion(uchar ver, uchar rev)
{
  __ucVersion = ver;
  __ucRevision = rev;
  __pInfo = ID3_LookupHeaderInfo(__ucVersion, __ucRevision);
}

void ID3_Header::SetDataSize(size_t newSize)
{
  __ulDataSize = newSize;
}

size_t ID3_Header::GetDataSize() const
{
  return __ulDataSize;
}

void ID3_Header::SetFlags(uint16 newFlags)
{
  __ulFlags = newFlags;
}

void ID3_Header::AddFlags(uint16 newFlags)
{
  __ulFlags |= newFlags;
}

void ID3_Header::RemoveFlags(uint16 newFlags)
{
  __ulFlags &= ~newFlags;
}

uint16 ID3_Header::GetFlags() const
{
  return __ulFlags;
}

uchar ID3_Header::GetVersion() const
{
  return __ucVersion;
}

uchar ID3_Header::GetRevision() const
{
  return __ucRevision;
}

void ID3_Header::Clear()
{
  __ucVersion = 0;
  __ucRevision = 0;
  __ulDataSize = 0;
  __ulFlags = 0;
  //__pInfo = 0;
}

ID3_Header &ID3_Header::operator=( const ID3_Header &hdr )
{
  Copy(hdr);
  return *this;
}

void ID3_Header::Copy(const ID3_Header &hdr)
{
  if (this != &hdr)
  {
    SetVersion(hdr.GetVersion(), hdr.GetRevision());
    SetDataSize(hdr.GetDataSize());
    SetFlags(hdr.GetFlags());
    __pInfo = hdr.__pInfo;
  }
}

// $Log: header.cpp,v $
// Revision 1.11  2000/01/04 15:42:49  eldamitri
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
// Revision 1.10  1999/12/27 05:43:28  scott
// (ID3_Header): Updated for new version constants.
// (AddFlags): Added implementation.  Adds flag(s) to current flags.
// (RemoveFlags): Added implementation.  Removes flag(s) from current
// flags.
// (GetVersion): Added implementation.  Returns version number of header.
// (GetRevision): Added implementation.  Returns revision of header.
// (Clear): Added implementation.  Clears contents of header.
// (operator=): Added implementation. Assignment operator.
// (Copy): Added implementation. Copies content of another header.
//
// Revision 1.9  1999/12/26 15:11:13  scott
// (GetDataSize): Added implementation.
// (GetFlags): Added implementation.
//
// Revision 1.8  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.7  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:18:46  scott
// Added include for config.h.  Minor code cleanup.  Made private
// member variable names more descriptive.  Fixed bug I introduced in
// logic of checking version/revision numbers in
// ID3_LookupHeaderInfo, and cleaned up the code in the process.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
