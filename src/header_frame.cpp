// $Id: header_frame.cpp,v 1.3 2000/05/01 13:05:07 eldamitri Exp $

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
