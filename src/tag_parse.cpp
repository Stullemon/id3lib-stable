// $Id: tag_parse.cpp,v 1.7 2000/05/02 21:55:38 eldamitri Exp $

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

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <zlib.h>
#include "tag.h"
#include "uint28.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_Elem *ID3_Tag::GetLastElem(ID3_Elem *list)
{
  ID3_Elem *last;
  
  last = list;
  
  while (last != NULL && last->pNext != NULL)
  {
    last = last->pNext;
  }
    
  return last;
}

void ID3_Tag::AddBinary(uchar *buffer, luint size)
{
  if (0 == size)
  {
    ID3_THROW(ID3E_NoData);
  }

  uchar *newBin = new uchar[size];
  if (NULL == newBin)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  ID3_Elem *elem, *lastElem;
      
  memcpy(newBin, buffer, size);
      
  elem = new ID3_Elem;
  if (NULL == elem)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  elem->pNext = NULL;
  elem->pFrame = NULL;
  elem->acBinary = newBin;
  elem->bTagOwns = true;
        
  lastElem = GetLastElem(__pBinaryList);
        
  if (NULL == lastElem)
  {
    __pBinaryList = elem;
  }
  else
  {
    lastElem->pNext = elem;
  }
    
  return ;
}

void ID3_Tag::ExpandBinaries(uchar *buffer, luint size)
{
  ID3_FrameHeader frHeader;
  luint posn = 0;
  
  // TODO: 6 is a magic number!  Find a suitable constant
  while (posn < (size - 6) && buffer[posn] != 0)
  {
    frHeader.SetSpec(this->GetSpec());
    posn += frHeader.Parse(&buffer[posn], size - posn);

    // firstly, let's check to see if we are parsing a CDM.
    if (frHeader.GetFrameID() != ID3FID_METACOMPRESSION)
    {
      AddBinary(&buffer[posn - frHeader.Size()], 
                frHeader.Size() + frHeader.GetDataSize());
      posn += frHeader.GetDataSize();
    }
    else
    {
      // If so, let's expand it out now.
      // if the method was zlib
      if ('z' == buffer[posn])
      {
        uint32 expandedSize = ParseNumber(&buffer[posn+1]);
        
        uchar *expBin = new uchar[expandedSize];
        if (NULL == expBin)
        {
          ID3_THROW(ID3E_NoMemory);
        }

        uncompress(expBin, (luint *) &expandedSize, 
                   &buffer[posn + 1 + sizeof(luint)], 
                   frHeader.GetDataSize() - sizeof(luint) - 1);
          
        ExpandBinaries(expBin, expandedSize);
          
        posn += frHeader.GetDataSize();
          
        delete[] expBin;
      }
    }
  }
}

void ID3_Tag::ProcessBinaries(ID3_FrameID whichFrame, bool attach)
{
  ID3_FrameHeader frHeader;
  ID3_Elem *cur = __pBinaryList;

  frHeader.SetSpec(this->GetSpec());
  
  while (cur != NULL && cur->acBinary)
  {
    ID3_FrameID id;
    ID3_Frame *frame;
    luint posn;
    luint extras = 0;
    uint32 expandedSize = 0;
    uchar groupingID = 0;
    uchar encryptionID = 0;
    bool bShouldAttach = attach;
    bool bReadOnly = false;
    
    posn = frHeader.Parse(cur->acBinary, 0);
    frHeader.SetSpec(this->GetSpec());
    
    if (frHeader.GetCompression())
    {
      expandedSize = ParseNumber(&(cur->acBinary[posn]));
      extras += sizeof(luint);
    }
    
    if (frHeader.GetEncryption())
    {
      encryptionID = cur->acBinary[posn];
      posn++, extras++;
    }
    
    if (frHeader.GetGrouping())
    {
      groupingID = cur->acBinary[posn];
      posn++, extras++;
    }

    bReadOnly = ((frHeader.GetReadOnly()) != 0);

    id = frHeader.GetFrameID();
    
    if ((id != whichFrame && ID3FID_NOFRAME != whichFrame) ||
        ID3FID_NOFRAME == id)
    {
      cur = cur->pNext;
    }
    else
    {
      uchar *expBin = NULL;
      
      if (frHeader.GetCompression())
      {
        expBin = new uchar[expandedSize];
        if (NULL == expBin)
        {
          ID3_THROW(ID3E_NoMemory);
        }
          
        uncompress(expBin, (luint *) &expandedSize, 
                   &cur->acBinary[posn + sizeof(luint)], 
                   frHeader.GetDataSize() - extras);
      }
      
      frame = new ID3_Frame(frHeader);
      if (NULL == frame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
        
      try 
      {
        if (NULL == expBin)
        {
          frame->Parse(&cur->acBinary[posn], frHeader.GetDataSize() - extras);
        }
        else
        {
          frame->Parse(expBin, expandedSize);
          delete[] expBin;
        }
        
      }
      catch (ID3_Error&)
      {
        // There's been an error in the parsing of the frame.  It shouldn't be
        // attached.

        // TODO: This should be logged somehow so that the user can determine
        // how many frames were parsed correctly and how many weren't 
        // cerr << "xxx Error occurred: " << err.GetErrorDesc() << endl;
        bShouldAttach = false;
        if (NULL != expBin)
        {
          delete [] expBin;
        }
      }
      
      if (!bShouldAttach)
      {
        // if not, delete it
        delete frame;
      }
      else
      {
        // if, after all is said and done, we are still supposed to attach our
        // newly parsed frame to the tag, do so
        ID3_Elem 
          *elem     = new ID3_Elem, 
          *lastElem = GetLastElem(__pFrameList);
        if (NULL == elem)
        {
          ID3_THROW(ID3E_NoMemory);
        }
        elem->pNext    = NULL;
        elem->pFrame   = frame;
        elem->acBinary = NULL;
        elem->bTagOwns = true;
        
        if (NULL == lastElem)
        {
          __pFrameList = elem;
        }
        else
        {
          lastElem->pNext = elem;
        }
      }
      
      ID3_Elem *temp = cur;
      cur = cur->pNext;
      
      RemoveFromList(temp, &__pBinaryList);
    }
  }
  
  return ;
}


  /** Turns a binary tag into a series of ID3_Frame objects attached to the
   ** tag.
   ** 
   ** \code
   **   ID3_Tag myTag;
   **   uchar header[ID3_TAGHEADERSIZE];
   **   uchar *buffer;
   **   luint tagSize;
   ** 
   **   // get ID3_TAGHEADERSIZE from a socket or somewhere
   **   // ...
   ** 
   **   if ((tagSize = ID3_IsTagHeader(ourSourceBuffer)) > -1)
   **   {
   **     // read a further 'tagSize' bytes in
   **     // from our data source
   **     // ...
   **   
   **     if (buffer = new uchar[tagSize])
   **     {
   **       // now we will call ID3_Tag::Parse()
   **       // with these values (explained later)
   **       myTag.Parse(header, buffer);
   **       
   **       // do something with the objects,
   **       // like look for titles, artists, etc.
   **       // ...
   **       
   **       // free the buffer
   **       delete [] buffer;
   **     }
   **   }
   ** \endcode
   ** 
   ** @see ID3_Frame
   ** @param header The byte header read in from the data source.
   ** @param buffer The remainder of the tag (not including the data source) 
   **               read in from the data source.
   **/
void ID3_Tag::Parse(uchar header[ID3_TAGHEADERSIZE], uchar *buffer)
{
  luint tagSize = 0;
  uint28 temp = &header[6];
  luint posn = 0;
  ID3_V2Spec prev_spec = this->GetSpec();
  
  Clear();
  
  tagSize = temp.to_uint32();
  this->SetSpec(ID3_VerRevToV2Spec(header[3], header[4]));
  
  if (header[5] & ID3_TagHeader::UNSYNC)
  {
    tagSize = ReSync(buffer, tagSize);
  }
  
  // okay, if we are ID3v2.2.1, then let's skip over the extended header for now
  // because I am lazy
  if (this->GetSpec() == ID3V2_2_1)
  {
    if (header[5] & ID3_TagHeader::EXTENDED)
    {
      uint32 extSize = ParseNumber(buffer);
      posn = extSize + sizeof(luint);
    }
  }
    
  // okay, if we are 3.00, then let's actually parse the extended header (for
  // now, we skip it because we are lazy)
  if (this->GetSpec() == ID3V2_3_0)
  {
    if (header[5] & ID3_TagHeader::EXTENDED)
    {
      uint32 extSize = ParseNumber(buffer);
      posn = extSize + sizeof(luint);
    }
  }
    
  // this call will convert the binary data block (tag) into a linked list of
  // binary frames
  ExpandBinaries(&buffer[posn], tagSize);
  
  // let's parse the CRYPTO frames. the 'false' parameter means "don't attach
  // the frame to the tag when processed".  This is because we have installed a
  // parsing handler for the crypto reg frame.  This is a default parameter -
  // if the frame type has a custom parsing handler, that handler will tell
  // ID3Lib whether to attach or not.
  ProcessBinaries(ID3FID_CRYPTOREG, false);
  
  // let's parse the GROUPING frames. the 'false' parameter means "don't attach
  // the frame to the tag when processed".  This is because we have installed a
  // parsing handler for the crypto reg frame.  This is a default parameter -
  // if the frame type has a custom parsing handler, that handler will tell
  // ID3Lib whether to attach or not.
  ProcessBinaries(ID3FID_GROUPINGREG, false);
  
  // let's parse the rest of the binaries
  ProcessBinaries();
  
  // reset the version parameters which were in effect before the parse
  SetSpec(prev_spec);
  
  // set the flag which says that the tag hasn't changed
  __bHasChanged = false;
  
  return ;
}


luint ID3_Tag::ParseFromHandle(void)
{
  luint size = 0;

  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_NoData);
  }

  if (__ulTagsToParse & ID3TT_ID3V2)
  {
    if (fseek(__fFileHandle, 0, SEEK_SET) != 0)
    {
      ID3_THROW_DESC(ID3E_NoFile, 
                     "ID3_Tag::ParseFromHandle: Ack! Couldn't seek");
    }
    
    uchar header[ID3_TAGHEADERSIZE];
    if (fread(header, 1, sizeof(header), __fFileHandle) == 0)
    {
      ID3_THROW_DESC(ID3E_NoFile, 
                     "ID3_Tag::ParseFromHandle: Ack! Couldn't read");
    }
    
    lsint tagSize = ID3_IsTagHeader(header);
    if (tagSize > 0)
    {
      uchar* bin = new uchar[tagSize];
      if (NULL == bin)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      
      if (fread(bin, 1, tagSize, __fFileHandle) == 0)
      {
        ID3_THROW_DESC(ID3E_NoFile, 
                       "ID3_Tag::ParseFromHandle: Ack! Couldn't read");
      }
      
      Parse(header, bin);
      size = tagSize;
      
      delete[] bin;
    }
  }
    
  if (__ulTagsToParse & ID3TT_LYRICS)
  {
    ParseLyrics3();
  }
  
  if (__ulTagsToParse & ID3TT_ID3V1)
  {
    ParseID3v1();
  }
    
  return size;

}
