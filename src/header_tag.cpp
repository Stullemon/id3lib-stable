// $Id: header_tag.cpp,v 1.6 2000/05/03 14:05:34 eldamitri Exp $

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

#include "header_tag.h"
#include "uint28.h"
#include "utils.h"
#include "tag.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

bool ID3_TagHeader::SetSpec(const ID3_V2Spec spec)
{
  bool changed = this->ID3_Header::SetSpec(spec);
  if (changed)
  {
    if (__info)
    {
      __flags.set(EXPERIMENTAL, __info->is_experimental);
      __flags.set(EXTENDED,     __info->is_extended);
    }
  }
  return changed;
}

size_t ID3_TagHeader::Size() const
{
  size_t bytesUsed = ID3_TagHeader::SIZE;
  
  if (__info->is_extended)
  {
    bytesUsed += __info->extended_bytes + sizeof(uint32);
  }
  
  return bytesUsed;
}


size_t ID3_TagHeader::Render(uchar *buffer) const
{
  size_t size = 0;
  
  memcpy(&buffer[size], (uchar *) ID, strlen(ID));
  size += strlen(ID);
  
  buffer[size++] = ID3_V2SpecToVer(this->GetSpec());
  buffer[size++] = ID3_V2SpecToRev(this->GetSpec());
  
  // set the flags byte in the header
  buffer[size++] = static_cast<uchar>(__flags.get() & MASK8);
  
  uint28 size28(this->GetDataSize());
  size28.Render(&buffer[size]);
  size += sizeof(uint32);

  // now we render the extended header
  if (__flags.test(EXTENDED))
  {
    id3::RenderNumber(&buffer[size], __info->extended_bytes);
    size += sizeof(uint32);
  }
  
  return size;
}

size_t ID3_TagHeader::Parse(const uchar* data, size_t data_size)
{
  if (!(ID3_IsTagHeader(data) > 0))
  {
    return 0;
  }

  size_t size = SIZE;

  // The spec version is determined with the MAJOR and MINOR OFFSETs
  uchar major = data[MAJOR_OFFSET];
  uchar minor = data[MINOR_OFFSET];
  this->SetSpec(ID3_VerRevToV2Spec(major, minor));

  // Get the flags at the appropriate offset
  __flags.set(static_cast<ID3_Flags::TYPE>(data[FLAGS_OFFSET]));

  // set the data size
  this->SetDataSize(id3::ParseNumber(&data[SIZE_OFFSET], sizeof(uint32)));
  
  if (__flags.test(EXTENDED))
  {
    // need to do something here
  }
  
  return size;
}
