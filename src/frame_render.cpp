// $Id: frame_render.cpp,v 1.9 2000/05/12 21:09:40 eldamitri Exp $

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
#include <zlib.h>
#include "tag.h"
#include "utils.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

// at the moment, Render can't be const, since it resets some variables in
// the header and the fields.  If compression doesn't actually produce something
// smaller, it will turn off the compression flag in the header.  
size_t ID3_Frame::Render(uchar *buffer)
{
  size_t bytesUsed = 0;
  
  if (NULL == buffer)
  {
    ID3_THROW(ID3E_NoBuffer);
  }
  
  size_t extras = 0;
  bool didCompress = false;
    
  bytesUsed += __hdr.Size();
    
  // here is where we include things like grouping IDs and crypto IDs
  if (strlen(__encryption_id) > 0)
  {
    buffer[bytesUsed] = __encryption_id[0];
    bytesUsed++;
    extras++;
  }
    
  if (strlen(__grouping_id) > 0)
  {
    buffer[bytesUsed] = __grouping_id[0];
    bytesUsed++;
    extras++;
  }
    
  ID3_TextEnc enc = ID3TE_ASCII;
    
  for (ID3_Field** fi = __fields; fi != __fields + __num_fields; fi++)
  {
    if ((*fi)->GetID() == ID3FN_TEXTENC)  
    {
      enc = static_cast<ID3_TextEnc>((*fi)->Get());  
    }
    
    if (*fi && (*fi)->InScope(this->GetSpec()))
    {
      (*fi)->SetEncoding(enc);
      bytesUsed += (*fi)->Render(&buffer[bytesUsed]);
    }
  }
    
  // if we can compress frames individually and we have been asked to compress
  // the frames
  if (__hdr.GetCompression() && __hdr.GetSpec() >= ID3V2_3_0)
  {
      
    bytesUsed -= __hdr.Size();
      
    // unsigned long needed for compress
    unsigned long newFrameSize = bytesUsed + (bytesUsed / 10) + 12;
      
    uchar* newTemp = new uchar[newFrameSize];
    if (NULL == newTemp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    if (compress(newTemp, &newFrameSize, &buffer[__hdr.Size() + extras],
                 bytesUsed - extras) != Z_OK)
    {
      ID3_THROW(ID3E_zlibError);
    }

    // if the compression actually saves space
    if ((newFrameSize + sizeof(uint32)) < bytesUsed)
    {
      size_t posn = __hdr.Size();
      extras += sizeof(uint32);
            
      memcpy(&buffer[posn + sizeof(uint32)], newTemp, newFrameSize);
        
      bytesUsed += newFrameSize + RenderNumber(&buffer[posn], bytesUsed);
      didCompress = true;
    }
          
    bytesUsed += __hdr.Size();
        
    delete[] newTemp;
  }
    
  // perform any encryption here
  if (strlen(__encryption_id))
  {
  }
    
  // determine which flags need to be set
  __hdr.SetCompression(didCompress);
  __hdr.SetEncryption(strlen(__encryption_id) > 0);
  __hdr.SetGrouping(strlen(__grouping_id) > 0);
      
  __hdr.SetDataSize(bytesUsed - __hdr.Size());
  __hdr.Render(buffer);
  __changed = false;
    
  return bytesUsed;
}
