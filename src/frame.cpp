// $Id: frame.cpp,v 1.8 2000/05/02 21:58:23 eldamitri Exp $

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
#include "tag.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_Frame::ID3_Frame(ID3_FrameID id)
  : __bHasChanged(false),
    __auiFieldBits(NULL),
    __ulNumFields(0),
    __apFields(NULL)
{
  __sEncryptionID[0] = '\0';
  __sGroupingID[0]   = '\0';
  
  InitFieldBits();
  SetID(id);
}

ID3_Frame::ID3_Frame(const ID3_FrameHeader &hdr)
  : __bHasChanged(false),
    __auiFieldBits(NULL),
    __ulNumFields(0),
    __apFields(NULL),
    __FrmHdr(hdr)
{
  __sEncryptionID[0] = '\0';
  __sGroupingID[0]   = '\0';
  
  this->InitFieldBits();
  this->InitFields();
}

ID3_Frame::ID3_Frame(const ID3_Frame& frame)
  : __bHasChanged(false),
    __auiFieldBits(NULL),
    __ulNumFields(0),
    __apFields(NULL)
{
  __sEncryptionID[0] = '\0';
  __sGroupingID[0]   = '\0';
  
  InitFieldBits();
  *this = frame;
}

void ID3_Frame::InitFieldBits()
{
  luint lWordsForFields =
    (((luint) ID3FN_LASTFIELDID) - 1) / (sizeof(luint) * 8);
  
  if ((((luint) ID3FN_LASTFIELDID) - 1) % (sizeof(luint) * 8) != 0)
  {
    lWordsForFields++;
  }
   
  if (__auiFieldBits != NULL)
  {
    delete [] __auiFieldBits;
  }
  __auiFieldBits = new luint[lWordsForFields];
  if (NULL == __auiFieldBits)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  for (luint i = 0; i < lWordsForFields; i++)
  {
    __auiFieldBits[i] = 0;
  }
}

ID3_Frame::~ID3_Frame(void)
{
  Clear();
  
  if (__auiFieldBits != NULL)
  {
    delete [] __auiFieldBits;
  }
}


void ID3_Frame::Clear(void)
{
  if (__ulNumFields > 0 && NULL != __apFields)
  {
    for (luint i = 0; i < __ulNumFields; i++)
    {
      delete __apFields[i];
    }
      
    delete [] __apFields;
    
    __bHasChanged = true;
  }

  __FrmHdr.Clear();
  __sEncryptionID[0] = '\0';
  __sGroupingID[0]   = '\0';
  __ulNumFields      = 0;
  __apFields         = NULL;
}

void ID3_Frame::InitFields()
{
  const ID3_FrameDef* info = __FrmHdr.GetFrameDef();
  if (NULL == info)
  {
    ID3_THROW(ID3E_InvalidFrameID);
  }
      
  __ulNumFields = 0;
      
  while (info->aeFieldDefs[__ulNumFields].eID != ID3FN_NOFIELD)
  {
    __ulNumFields++;
  }
      
  __apFields = new ID3_Field * [__ulNumFields];
  if (NULL == __apFields)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  for (luint i = 0; i < __ulNumFields; i++)
  {
    __apFields[i] = new ID3_Field;
    if (NULL == __apFields[i])
    {
      ID3_THROW(ID3E_NoMemory);
    }

    __apFields[i]->__eName        = info->aeFieldDefs[i].eID;
    __apFields[i]->__eType        = info->aeFieldDefs[i].eType;
    __apFields[i]->__ulFixedLength = info->aeFieldDefs[i].ulFixedLength;
    __apFields[i]->__eSpecBegin   = info->aeFieldDefs[i].eSpecBegin;
    __apFields[i]->__eSpecEnd     = info->aeFieldDefs[i].eSpecEnd;
    __apFields[i]->__ulFlags      = info->aeFieldDefs[i].ulFlags;
            
    // tell the frame that this field is present
    BS_SET(__auiFieldBits, __apFields[i]->__eName);
  }
        
  __bHasChanged = true;
}

void ID3_Frame::SetID(ID3_FrameID id)
{
  Clear();
  
  if (id != ID3FID_NOFRAME)
  {
    __FrmHdr.SetFrameID(id);
    this->InitFields();
  }
}


ID3_FrameID ID3_Frame::GetID(void) const
{
  return __FrmHdr.GetFrameID();
}


bool ID3_Frame::SetSpec(const ID3_V2Spec spec)
{
  return __FrmHdr.SetSpec(spec);
}

ID3_V2Spec ID3_Frame::GetSpec() const
{
  return __FrmHdr.GetSpec();
}

lsint ID3_Frame::FindField(ID3_FieldID fieldName) const
{
  
  if (BS_ISSET(__auiFieldBits, fieldName))
  {
    for (lsint num = 0; num < (lsint) __ulNumFields; num++)
    {
      if (__apFields[num]->__eName == fieldName)
      {
        return num;
      }
    }
  }

  return -1;
}

ID3_Field& ID3_Frame::Field(ID3_FieldID fieldName) const
{
  lsint fieldNum = FindField(fieldName);
  
  if (fieldNum < 0)
  {
    ID3_THROW(ID3E_FieldNotFound);
  }
    
  return *__apFields[fieldNum];
}

void ID3_Frame::UpdateFieldDeps(void)
{
  for (luint i = 0; i < __ulNumFields; i++)
  {
    if (__apFields[i]->__ulFlags & ID3FF_ADJUSTEDBY)
    {
      switch(__apFields[i]->__eType)
      {
        case ID3FTY_BITFIELD:
        {
          //luint value = 0;
          
          // now find the field on which this field is dependent and get a
          // copy of the value of that field.  then adjust the fixedLength of
          // this field to that value / 8.
          break;
        }
        
        default:
          break;
      }
    }
  }
  
  return ;
}


void ID3_Frame::UpdateStringTypes(void)
{
  for (luint i = 0; i < __ulNumFields; i++)
  {
    if (__apFields[i]->__ulFlags & ID3FF_ADJUSTENC)
    {
      ID3_TextEnc enc;
      ID3_FieldType newType;
      
      enc = (ID3_TextEnc) Field(ID3FN_TEXTENC).Get();
      
      switch(enc)
      {
        case ID3TE_ASCII:
          newType = ID3FTY_ASCIISTRING;
          break;
          
        case ID3TE_UNICODE:
          newType = ID3FTY_UNICODESTRING;
          break;
          
        default:
          newType = ID3FTY_ASCIISTRING;
          break;
      }
      
      __apFields[i]->__eType = newType;
    }
  }
  
  return ;
}


luint ID3_Frame::Size(void)
{
  luint bytesUsed = 0;
  bytesUsed += __FrmHdr.Size();
  
  if (strlen(__sEncryptionID))
  {
    bytesUsed++;
  }
    
  if (strlen(__sGroupingID))
  {
    bytesUsed++;
  }
    
  // this call is to tell the string fields what they should be rendered/parsed
  // as (ASCII or Unicode)
  UpdateStringTypes();
  
  for (luint i = 0; i < __ulNumFields; i++)
  {
    __apFields[i]->SetSpec(__FrmHdr.GetSpec());
    bytesUsed += __apFields[i]->BinSize();
  }
  
  return bytesUsed;
}


bool ID3_Frame::HasChanged(void) const
{
  bool changed = __bHasChanged;
  
  if (! changed)
  {
    for (luint i = 0; i < __ulNumFields && !changed; i++)
    {
      changed = __apFields[i]->HasChanged();
    }
  }
  
  return changed;
}

ID3_Frame &
ID3_Frame::operator=( const ID3_Frame &rFrame )
{
  if (this != &rFrame)
  {
    ID3_FrameID eID = rFrame.GetID();
    SetID(eID);
    for (size_t nIndex = 0; nIndex < __ulNumFields; nIndex++)
    {
      if (rFrame.__apFields[nIndex] != NULL)
      {
        *(__apFields[nIndex]) = *(rFrame.__apFields[nIndex]);
      }
    }
  }
  return *this;
}
