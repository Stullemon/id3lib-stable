// $Id: header_tag.cpp,v 1.4 2000/05/01 13:05:07 eldamitri Exp $

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
#include "int28.h"
#include "header_tag.h"
#include "error.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

// Analyses a buffer to determine if we have a valid ID3v2 tag header.
// If so, return the number of bytes (starting _after_ the header) to
// read so we get all of the tag

lsint ID3_IsTagHeader(uchar header[ID3_TAGHEADERSIZE])
{
  lsint tagSize = -1;
  
  if ((memcmp(ID3_TAGID, header, ID3_TAGIDSIZE) == 0) &&
      (header[ID3_TAGIDSIZE] <= ID3_V2SpecToVer(ID3V2_LATEST)))
  {
    int28 temp = &header[6];
    tagSize = temp.get();
  }
    
  return tagSize;
}


size_t ID3_TagHeader::Size(void)
{
  size_t bytesUsed = ID3_TAGHEADERSIZE;
  
  if (__pInfo->bHasExtHeader)
  {
    bytesUsed += __pInfo->ulExtHeaderBytes + sizeof(uint32);
  }
    
  return bytesUsed;
}


size_t ID3_TagHeader::Render(uchar *buffer)
{
  size_t bytesUsed = 0;
  
  memcpy(&buffer[bytesUsed], (uchar *) ID3_TAGID, strlen(ID3_TAGID));
  bytesUsed += strlen(ID3_TAGID);
  
  buffer[bytesUsed++] = ID3_V2SpecToVer(this->GetSpec());
  buffer[bytesUsed++] = ID3_V2SpecToRev(this->GetSpec());
  
  // do the automatic flags
  if (__pInfo->bSetExpBit)
  {
    __ulFlags |= ID3HF_EXPERIMENTAL;
  }
    
  if (__pInfo->bHasExtHeader)
  {
    __ulFlags |= ID3HF_EXTENDEDHEADER;
  }
    
  // set the flags byte in the header
  buffer[bytesUsed++] = (uchar)(__ulFlags & MASK8);
  
  int28 temp = __ulDataSize;
  
  for (size_t i = 0; i < sizeof(uint32); i++)
  {
    buffer[bytesUsed++] = temp[i];
  }
    
  // now we render the extended header
  if (__pInfo->bHasExtHeader)
  {
    RenderNumber(&buffer[bytesUsed], __pInfo->ulExtHeaderBytes);
    bytesUsed += sizeof(uint32);
  }
  
  bytesUsed = Size();
  
  return bytesUsed;
}

ID3_TagHeader& ID3_TagHeader::operator=(const ID3_TagHeader& hdr)
{
  if (this != &hdr)
  {
    this->ID3_Header::operator=(hdr);
  }
  return *this;
}
