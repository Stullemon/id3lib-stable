// $Id: header.cpp,v 1.6 2000/05/01 13:05:07 eldamitri Exp $

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
