// $Id: tag_parse.cpp,v 1.19 2000/04/10 16:58:13 eldamitri Exp $

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
#include "int28.h"
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
    frHeader.SetVersion(__ucVersion, __ucRevision);
    
    posn += frHeader.Parse(&buffer[posn]);
    // firstly, let's check to see if we are parsing a CDM.
    if (strcmp(frHeader.GetTextID(), "CDM") != 0)
    {
      AddBinary(&buffer[posn - frHeader.Size()], 
                frHeader.Size() + frHeader.GetDataSize());
      posn += frHeader.GetDataSize();
    }
    else
    {
      //If so, let's expand it out now.
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
  
  return ;
}

void ID3_Tag::ProcessBinaries(ID3_FrameID whichFrame, bool attach)
{
  ID3_FrameHeader frHeader;
  ID3_Elem *cur = __pBinaryList;

  frHeader.SetVersion(__ucVersion, __ucRevision);
  
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
    
    posn = frHeader.Parse(cur->acBinary);
    
    if (frHeader.GetFlags() & ID3FL_COMPRESSION)
    {
      expandedSize = ParseNumber(&(cur->acBinary[posn]));
      extras += sizeof(luint);
    }
    
    if (frHeader.GetFlags() & ID3FL_ENCRYPTION)
    {
      encryptionID = cur->acBinary[posn];
      posn++, extras++;
    }
    
    if (frHeader.GetFlags() & ID3FL_GROUPING)
    {
      groupingID = cur->acBinary[posn];
      posn++, extras++;
    }

    bReadOnly = ((frHeader.GetFlags() & ID3FL_READONLY) != 0);

    id = frHeader.GetFrameID();
    
    if ((id != whichFrame && ID3FID_NOFRAME != whichFrame) ||
        ID3FID_NOFRAME == id)
    {
      cur = cur->pNext;
    }
    else
    {
      uchar *expBin = NULL;
      
      if (frHeader.GetFlags() & ID3FL_COMPRESSION)
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
      
      frame = new ID3_Frame;
      if (NULL == frame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
        
      frame->SetID(id);
      
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
        
        // here is where we call a special handler for this frame type if one
        // is specified in the frame definition
        {
          ID3_FrameDef *frameInfo = ID3_FindFrameDef(id);
          
          if (frameInfo != NULL && frameInfo->parseHandler != NULL)
          {
            bShouldAttach = frameInfo->parseHandler(frame);
          }
        }
      }
      catch (ID3_Error err)
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


void ID3_Tag::Parse(uchar header[ID3_TAGHEADERSIZE], uchar *buffer)
{
  luint tagSize = 0;
  int28 temp = &header[6];
  luint posn = 0;
  uchar prevVer = __ucVersion;
  uchar prevRev = __ucRevision;
  
  Clear();
  
  tagSize = temp.get();
  SetVersion(header[3], header[4]);
  
  if (header[5] & ID3HF_UNSYNC)
  {
    tagSize = ReSync(buffer, tagSize);
  }
    
  // okay, if we are 2.01, then let's skip over the extended header for now
  // because I am lazy
  if (2 == __ucVersion && 1 == __ucRevision)
  {
    if (header[5] & ID3HF_EXTENDEDHEADER)
    {
      uint32 extSize = ParseNumber(buffer);
      posn = extSize + sizeof(luint);
    }
  }
    
  // okay, if we are 3.00, then let's actually parse the extended header (for
  // now, we skip it because we are lazy)
  if (3 == __ucVersion && 0 == __ucRevision)
  {
    if (header[5] & ID3HF_EXTENDEDHEADER)
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
  SetVersion(prevVer, prevRev);
  
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

  uchar header[ID3_TAGHEADERSIZE];
  lsint tagSize;
    
  if (fseek(__fFileHandle, 0, SEEK_SET) != 0)
  {
    ID3_THROW_DESC(ID3E_NoFile, 
                   "ID3_Tag::ParseFromHandle: Ack! Couldn't seek");
  }
      
  if (fread(header, 1, sizeof(header), __fFileHandle) == 0)
  {
    ID3_THROW_DESC(ID3E_NoFile, 
                   "ID3_Tag::ParseFromHandle: Ack! Couldn't read");
  }

  tagSize = ID3_IsTagHeader(header);
  if (tagSize > 0)
  {
    uchar * bin;
        
    bin = new uchar[tagSize];
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
    
  if (__bParseLyrics3)
  {
    ParseLyrics3();
  }
  if (__bParseID3v1)
  {
    ParseID3v1();
  }
    
  return size;

}

// $Log: tag_parse.cpp,v $
// Revision 1.19  2000/04/10 16:58:13  eldamitri
// (ProcessBinaries): Minor change to prevent warning in windows.
//
// Revision 1.18  2000/04/09 22:46:04  eldamitri
// Minor structure change.
//
// Revision 1.17  2000/04/08 04:42:59  eldamitri
// Changed new ANSI-standard C++ include headers to old-style headers.
//
// Revision 1.16  2000/04/07 04:35:38  eldamitri
// Added optional parameters to Link to make parsing of id3v1/lyrics3
// tags optional.
//
// Revision 1.15  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.14  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.13  1999/12/27 05:29:03  scott
// (ExpandBinaries, ProcessBinaries): Moved header processing tasks to
// the header object, now a member of ID3_Frame.
//
// Revision 1.12  1999/12/26 15:11:57  scott
// (ExpandBinaries): Now uses ParseNumber, defined in misc_support.
// (ProcessBinaries): Now uses ParseNumber, defined in misc_support.
// (Parse): Now uses ParseNumber, defined in misc_support.
//
// Revision 1.11  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.10  1999/12/13 16:49:55  scott
// Minor code cleanup.
//
// Revision 1.9  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.8  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.7  1999/11/25 18:41:29  scott
// (ProcessBinaries): Minor code reorg.
//
// Revision 1.6  1999/11/16 22:50:37  scott
// * tag_parse.cpp
// (ProcessBinaries): Added try/catch block to catch any exceptions so
// that the parser can carry on parsing the rest of the frames if any
// particular frame is poorly encoded.  Need to add some sort of
// mechanism to determine how many frames exist, how many have been
// parsed correctly and how many have been parsed incorrectly.
//
// Revision 1.5  1999/11/15 20:20:55  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
