// $Id: header_frame.cpp,v 1.16 2000/04/10 20:58:14 eldamitri Exp $

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
#include "header_frame.h"
#include "error.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_FrameHeader::ID3_FrameHeader()
  : __pFrameDef(NULL)
{
}

ID3_FrameHeader::~ID3_FrameHeader()
{
  Clear();
}

void ID3_FrameHeader::SetUnknownFrame(const char* id)
{
  Clear();
  __pFrameDef = new ID3_FrameDef;
  if (NULL == __pFrameDef)
  {
  }
  __pFrameDef->eID = ID3FID_NOFRAME;
  __pFrameDef->bTagDiscard = false;
  __pFrameDef->bFileDiscard = false;
  __pFrameDef->parseHandler = NULL;
  __pFrameDef->aeFieldDefs = (ID3_FieldDef *) ID3_FieldDef::DEFAULT;
  if (strlen(id) <= 3)
  {
    strcpy(__pFrameDef->sShortTextID, id);
    strcpy(__pFrameDef->sLongTextID, "");
  }
  else
  {
    strncpy(__pFrameDef->sLongTextID, id, 4);
    strcpy(__pFrameDef->sShortTextID, "");
  }
  __bDynFrameDef = true;
}

void ID3_FrameHeader::SetFrameID(ID3_FrameID id)
{
  if (id == ID3FID_NOFRAME)
  {
    return;
  }
  Clear();
  __pFrameDef = ID3_FindFrameDef(id);
  if (__pFrameDef->bTagDiscard)
  {
    AddFlags(ID3FL_TAGALTER);
  }
        
  if (__pFrameDef->bFileDiscard)
  {
    AddFlags(ID3FL_FILEALTER);
  }
}

size_t ID3_FrameHeader::Size(void)
{
  return 
    __pInfo->ucFrameIDBytes   + 
    __pInfo->ucFrameSizeBytes + 
    __pInfo->ucFrameFlagsBytes;
}

size_t ID3_FrameHeader::Parse(uchar *buffer)
{
  size_t nSize = 0;
  char sTextID[5];

  strncpy(sTextID, (char *) buffer, __pInfo->ucFrameIDBytes);
  sTextID[__pInfo->ucFrameIDBytes] = '\0';
  nSize += __pInfo->ucFrameIDBytes;

  ID3_FrameID fid = ID3_FindFrameID(sTextID);
  if (ID3FID_NOFRAME == fid)
  {
    SetUnknownFrame(sTextID);
  }
  else
  {
    SetFrameID(fid);
  }

  SetDataSize(ParseNumber(&buffer[nSize], __pInfo->ucFrameSizeBytes));
  nSize += __pInfo->ucFrameSizeBytes;

  uint16 flags = GetFlags() | 
    (uint16) ParseNumber(&buffer[nSize], __pInfo->ucFrameFlagsBytes);
  SetFlags(flags);
  nSize += __pInfo->ucFrameFlagsBytes;
  
  return nSize;
}

size_t ID3_FrameHeader::Render(uchar *buffer)
{
  size_t bytesUsed = 0;

  if (NULL == __pFrameDef)
  {
    ID3_THROW(ID3E_InvalidFrameID);
  }
  char *sTextID;
  if (__pInfo->ucFrameIDBytes == strlen(__pFrameDef->sShortTextID))
  {
    sTextID = __pFrameDef->sShortTextID;
  }
  else
  {
    sTextID = __pFrameDef->sLongTextID;
  }

  memcpy(&buffer[bytesUsed], (uchar *) sTextID, __pInfo->ucFrameIDBytes);
  bytesUsed += __pInfo->ucFrameIDBytes;
    
  RenderNumber(&buffer[bytesUsed], __ulDataSize, __pInfo->ucFrameSizeBytes);
  bytesUsed += __pInfo->ucFrameSizeBytes;
    
  RenderNumber(&buffer[bytesUsed], __ulFlags, __pInfo->ucFrameFlagsBytes);
  bytesUsed += __pInfo->ucFrameFlagsBytes;

  return bytesUsed;
}

const char *
ID3_FrameHeader::GetTextID() const
{
  char *sTextID = "";
  if (NULL != __pFrameDef)
  {
    if (__pInfo->ucFrameIDBytes == strlen(__pFrameDef->sShortTextID))
    {
      sTextID = __pFrameDef->sShortTextID;
    }
    else
    {
      sTextID = __pFrameDef->sLongTextID;
    }
  }
  return sTextID;
}

ID3_FrameHeader& ID3_FrameHeader::operator=(const ID3_FrameHeader& hdr)
{
  if (this != &hdr)
  {
    Clear();
    this->ID3_Header::operator=(hdr);
    if (!hdr.__bDynFrameDef)
    {
      __pFrameDef = hdr.__pFrameDef;
    }
    else
    {
      __pFrameDef = new ID3_FrameDef;
      if (NULL == __pFrameDef)
      {
        // TODO: throw something here...
      }
      __pFrameDef->eID = hdr.__pFrameDef->eID;
      __pFrameDef->bTagDiscard = hdr.__pFrameDef->bTagDiscard;
      __pFrameDef->bFileDiscard = hdr.__pFrameDef->bFileDiscard;
      __pFrameDef->parseHandler = hdr.__pFrameDef->parseHandler;
      __pFrameDef->aeFieldDefs = hdr.__pFrameDef->aeFieldDefs;
      strcpy(__pFrameDef->sShortTextID, hdr.__pFrameDef->sShortTextID);
      strcpy(__pFrameDef->sLongTextID, hdr.__pFrameDef->sLongTextID);
      __bDynFrameDef = true;
    }
  }
  return *this;
}

ID3_FrameID ID3_FrameHeader::GetFrameID() const
{
  ID3_FrameID eID = ID3FID_NOFRAME;
  if (NULL != __pFrameDef)
  {
    eID = __pFrameDef->eID;
  }

  return eID;
}

const ID3_FrameDef *ID3_FrameHeader::GetFrameDef() const
{
  return __pFrameDef;
}

void ID3_FrameHeader::Clear()
{
  ID3_Header::Clear();
  if (__bDynFrameDef)
  {
    delete __pFrameDef;
  }
  __bDynFrameDef = false;
  __pFrameDef = NULL;
}

// $Log: header_frame.cpp,v $
// Revision 1.16  2000/04/10 20:58:14  eldamitri
// (operator=): Fixed bug - now returns *this.
//
// Revision 1.15  2000/04/10 16:57:28  eldamitri
// (Copy): removed.
// (operator=): Updated implementation so it isn't reliant on Copy.
//
// Revision 1.14  2000/04/09 22:42:19  eldamitri
// (ID3_FrameHeader): Added implementation.
// (SetUnknownFrame): Added implementation.
// (SetFrameID): Sanity checking inputs.
// (Parse): Now checks to make sure frame id isn't bogus.  If it is,
// creates an "unknown" frame.
// (Render): Fixed bug in short/long id determination logic
// (GetTextID): Fixed bug in short/long id determination logic
// (Copy): Handles case when copying a frame that has an "unknown" frame
// type.
// (Clear): Now deletes __pFrameDef is it was dynamically created.
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
// Revision 1.10  1999/12/27 05:40:24  scott
// (ID3_FrameHeader): Added default constructor.
// (SetFrameID): Modified to reflect internal representation change.  Now
// searches and stores frame definition associated with the frame id.
// (GetFrameInfo): Removed.
// (Parse): Added implementation.  Based on former GetFrameInfo.
// (Render): Modified to reflect internal representation change.
// (GetTextID): Added implementation.  Gets text id associated with frame
// header.
// (Copy): Added implementation.  Copies another frame header.
// (GetFrameID): Added implementation.  Gets frame id associated with
// frame header.
// (GetFrameDef): Added implementation.  Gets frame definition associated
// with frame header.
// (Clear): Added implementation.  Clears contents of frame header.
//
// Revision 1.9  1999/12/26 15:11:26  scott
// (GetFrameInfo): Now uses ParseNumber, defined in misc_support.
// (Render): Now uses RenderNumber, defined in misc_support.
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
// Revision 1.5  1999/11/15 20:19:08  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
