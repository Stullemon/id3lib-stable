// $Id: frame_render.cpp,v 1.5 2000/05/02 21:57:48 eldamitri Exp $

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
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

luint ID3_Frame::Render(uchar *buffer)
{
  luint bytesUsed = 0;
  
  if (NULL == buffer)
  {
    ID3_THROW(ID3E_NoBuffer);
  }

  luint extras = 0;
  bool didCompress = false;
    
  bytesUsed += __FrmHdr.Size();
    
  // here is where we include things like grouping IDs and crypto IDs
  if (strlen(__sEncryptionID) > 0)
  {
    buffer[bytesUsed] = __sEncryptionID[0];
    bytesUsed++;
    extras++;
  }
    
  if (strlen(__sGroupingID) > 0)
  {
    buffer[bytesUsed] = __sGroupingID[0];
    bytesUsed++;
    extras++;
  }
    
  // this call is to tell the string fields what they should be rendered/parsed
  // as (ASCII or Unicode)
  UpdateStringTypes();
    
  for (luint i = 0; i < __ulNumFields; i++)
  {
    __apFields[i]->SetSpec(__FrmHdr.GetSpec());
    bytesUsed += __apFields[i]->Render(&buffer[bytesUsed]);
  }
    
  // if we can compress frames individually and we have been asked to compress
  // the frames
  if (__FrmHdr.GetCompression() && __FrmHdr.GetSpec() >= ID3V2_3_0)
  {
      
    bytesUsed -= __FrmHdr.Size();
      
    luint newFrameSize = bytesUsed + (bytesUsed / 10) + 12;
      
    uchar* newTemp = new uchar[newFrameSize];
    if (NULL == newTemp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    if (compress(newTemp, &newFrameSize, &buffer[__FrmHdr.Size() + extras],
                 bytesUsed - extras) != Z_OK)
    {
      ID3_THROW(ID3E_zlibError);
    }

    // if the compression actually saves space
    if ((newFrameSize + sizeof(luint)) < bytesUsed)
    {
      luint posn;
            
      posn = __FrmHdr.Size();
      extras += sizeof(uint32);
            
      memcpy(&buffer[posn + sizeof(uint32)], newTemp, newFrameSize);
        
      RenderNumber(&buffer[posn], bytesUsed);
              
      bytesUsed = newFrameSize + sizeof(uint32);
      didCompress = true;
    }
          
    bytesUsed += __FrmHdr.Size();
        
    delete[] newTemp;
  }
    
  // perform any encryption here
  if (strlen(__sEncryptionID))
  {
  }
    
  // determine which flags need to be set
  __FrmHdr.SetCompression(didCompress);
  __FrmHdr.SetEncryption(strlen(__sEncryptionID) > 0);
  __FrmHdr.SetGrouping(strlen(__sGroupingID) > 0);
      
  __FrmHdr.SetDataSize(bytesUsed - __FrmHdr.Size());
  __FrmHdr.Render(buffer);
  __bHasChanged = false;
    
  return bytesUsed;
}
