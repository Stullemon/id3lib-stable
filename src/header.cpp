// $Id: header.cpp,v 1.5 2000/04/26 15:50:24 eldamitri Exp $

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

#include <string.h>
#include <memory.h>
#include "header.h"
#include "error.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_HeaderInfo ID3_SpecInfo[] =
{
  //             SIZEOF SIZEOF SIZEOF EXT    EXT  EXPERIM
  // V2 SPEC     FRID   FRSZ   FRFL   HEADER SIZE BIT
  {  ID3V2_2_0,  3,     3,     0,     false, 0,   false },
  {  ID3V2_2_1,  3,     3,     0,     true,  8,   true }, 
  {  ID3V2_3_0,  4,     4,     2,     false, 10,  false },
  {  ID3V2_UNKNOWN }
};
  
ID3_HeaderInfo *ID3_LookupHeaderInfo(ID3_V2Spec spec)
{
  ID3_HeaderInfo *info = NULL;
  for (size_t i = 0; ID3_SpecInfo[i].eSpec != ID3V2_UNKNOWN; i++)
  {
    if (ID3_SpecInfo[i].eSpec == spec)
    {
      info = &ID3_SpecInfo[i];
      break;
    }
  }
    
  return info;
}

ID3_HeaderInfo* ID3_LookupHeaderInfo(uchar ver, uchar rev)
{
  return ID3_LookupHeaderInfo(ID3_VerRevToV2Spec(ver, rev));
}

ID3_Header::ID3_Header(void)
{
  SetSpec(ID3V2_LATEST);
  __ulDataSize = 0;
  __ulFlags = 0;
}

void ID3_Header::SetSpec(const ID3_V2Spec spec)
{
  __spec = spec;
  __pInfo = ID3_LookupHeaderInfo(spec);
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

ID3_V2Spec ID3_Header::GetSpec() const
{
  return __spec;
}

void ID3_Header::Clear()
{
  __spec = ID3V2_UNKNOWN;
  __ulDataSize = 0;
  __ulFlags = 0;
  //__pInfo = 0;
}

ID3_Header &ID3_Header::operator=( const ID3_Header& hdr )
{
  if (this != &hdr)
  {
    SetSpec(hdr.GetSpec());
    SetDataSize(hdr.GetDataSize());
    SetFlags(hdr.GetFlags());
    __pInfo = hdr.__pInfo;
  }
  return *this;
}

// $Log: header.cpp,v $
// Revision 1.5  2000/04/26 15:50:24  eldamitri
// (ID3_Header): Made call to SetSpec have explicit parameter
// (SetSpec): Parameter now const
//
// Revision 1.4  2000/04/26 03:42:52  eldamitri
// - Replaced version/revision uchar combination with ID3_V2Spec enums
// - Deprecated {Get,Set}Version, GetRevision for {Get,Set}Spec
// - ID3_VerCtl enumeration deprecated in favor of using two ID3_V2Spec
//   enums to denote field scope
// - Replaced ID3v2_VERSION, ID3v2_REVISION constants with ID3V2_LATEST
//   enum
// - Use ID3V2_UNKNOWN enum rather than 0 for version, revision
//
// Revision 1.3  2000/04/20 03:49:42  eldamitri
// (ID3_LookupHeaderInfo): Minor update
//
// Revision 1.2  2000/04/18 22:11:53  eldamitri
// Moved header.cpp from src/id3/ to src/
//
// Revision 1.14  2000/04/10 16:57:18  eldamitri
// (Copy): removed.
// (operator=): Updated implementation so it isn't reliant on Copy.
//
// Revision 1.13  2000/04/08 04:40:26  eldamitri
// Changed new ANSI-standard C++ include headers to old-style headers.
//
// Revision 1.12  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.11  2000/01/04 15:42:49  eldamitri
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
