// $Id: header_frame.cpp,v 1.4 2000/05/02 21:56:39 eldamitri Exp $

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
#include "utils.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

void ID3_FrameHeader::SetUnknownFrame(const char* id)
{
  Clear();
  __frame_def = new ID3_FrameDef;
  if (NULL == __frame_def)
  {
  }
  __frame_def->eID = ID3FID_NOFRAME;
  __frame_def->bTagDiscard = false;
  __frame_def->bFileDiscard = false;
  __frame_def->parseHandler = NULL;
  __frame_def->aeFieldDefs = (ID3_FieldDef *) ID3_FieldDef::DEFAULT;
  if (strlen(id) <= 3)
  {
    strcpy(__frame_def->sShortTextID, id);
    strcpy(__frame_def->sLongTextID, "");
  }
  else
  {
    strncpy(__frame_def->sLongTextID, id, 4);
    strcpy(__frame_def->sShortTextID, "");
  }
  __dyn_frame_def = true;
}

bool ID3_FrameHeader::SetFrameID(ID3_FrameID id)
{
  if (id == ID3FID_NOFRAME || id == this->GetFrameID())
  {
    return false;
  }
  this->Clear();
  __frame_def = ID3_FindFrameDef(id);
  if (__frame_def->bTagDiscard)
  {
    __flags.add(TAGALTER);
  }
  
  if (__frame_def->bFileDiscard)
  {
    __flags.add(FILEALTER);
  }
  return true;
}

size_t ID3_FrameHeader::Size() const
{
  if (!__info)
  {
    return 0;
  }
  return 
    __info->frame_bytes_id   + 
    __info->frame_bytes_size + 
    __info->frame_bytes_flags;
}

size_t ID3_FrameHeader::Parse(const uchar *buffer, size_t size)
{
  size_t nSize = 0;
  char sTextID[5];

  if (!__info)
  {
    return 0;
  }

  strncpy(sTextID, (char *) buffer, __info->frame_bytes_id);
  sTextID[__info->frame_bytes_id] = '\0';
  nSize += __info->frame_bytes_id;

  ID3_FrameID fid = ID3_FindFrameID(sTextID);
  if (ID3FID_NOFRAME == fid)
  {
    this->SetUnknownFrame(sTextID);
  }
  else
  {
    this->SetFrameID(fid);
  }

  this->SetDataSize(id3::ParseNumber(&buffer[nSize], __info->frame_bytes_size));
  nSize += __info->frame_bytes_size;

  __flags.add(id3::ParseNumber(&buffer[nSize], __info->frame_bytes_flags));
  nSize += __info->frame_bytes_flags;
  
  return nSize;
}

size_t ID3_FrameHeader::Render(uchar *buffer) const
{
  size_t size = 0;

  if (NULL == __frame_def)
  {
    // TODO: log this
    return 0;
    //ID3_THROW(ID3E_InvalidFrameID);
  }
  char *sTextID;
  if (__info->frame_bytes_id == strlen(__frame_def->sShortTextID))
  {
    sTextID = __frame_def->sShortTextID;
  }
  else
  {
    sTextID = __frame_def->sLongTextID;
  }

  memcpy(&buffer[size], (uchar *) sTextID, __info->frame_bytes_id);
  size += __info->frame_bytes_id;
    
  id3::RenderNumber(&buffer[size], __data_size, __info->frame_bytes_size);
  size += __info->frame_bytes_size;
    
  id3::RenderNumber(&buffer[size], __flags.get(), __info->frame_bytes_flags);
  size += __info->frame_bytes_flags;

  return size;
}

const char* ID3_FrameHeader::GetTextID() const
{
  char *sTextID = "";
  if (__info && __frame_def)
  {
    if (__info->frame_bytes_id == strlen(__frame_def->sShortTextID))
    {
      sTextID = __frame_def->sShortTextID;
    }
    else
    {
      sTextID = __frame_def->sLongTextID;
    }
  }
  return sTextID;
}

ID3_FrameHeader& ID3_FrameHeader::operator=(const ID3_FrameHeader& hdr)
{
  if (this != &hdr)
  {
    this->Clear();
    this->ID3_Header::operator=(hdr);
    if (!hdr.__dyn_frame_def)
    {
      __frame_def = hdr.__frame_def;
    }
    else
    {
      __frame_def = new ID3_FrameDef;
      if (NULL == __frame_def)
      {
        // TODO: throw something here...
      }
      __frame_def->eID = hdr.__frame_def->eID;
      __frame_def->bTagDiscard = hdr.__frame_def->bTagDiscard;
      __frame_def->bFileDiscard = hdr.__frame_def->bFileDiscard;
      __frame_def->parseHandler = hdr.__frame_def->parseHandler;
      __frame_def->aeFieldDefs = hdr.__frame_def->aeFieldDefs;
      strcpy(__frame_def->sShortTextID, hdr.__frame_def->sShortTextID);
      strcpy(__frame_def->sLongTextID, hdr.__frame_def->sLongTextID);
      __dyn_frame_def = true;
    }
  }
  return *this;
}

ID3_FrameID ID3_FrameHeader::GetFrameID() const
{
  ID3_FrameID eID = ID3FID_NOFRAME;
  if (NULL != __frame_def)
  {
    eID = __frame_def->eID;
  }

  return eID;
}

const ID3_FrameDef *ID3_FrameHeader::GetFrameDef() const
{
  return __frame_def;
}

bool ID3_FrameHeader::Clear()
{
  bool changed = this->ID3_Header::Clear();
  if (__dyn_frame_def)
  {
    delete __frame_def;
    __dyn_frame_def = false;
    changed = true;
  }
  if (__frame_def)
  {
    __frame_def = NULL;
    changed = true;
  }
  return changed;
}
