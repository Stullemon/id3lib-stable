// $Id: frame.cpp,v 1.12 2000/01/04 15:42:49 eldamitri Exp $
// 
// This program is free software; you can distribute it and/or modify it under
// the terms discussed in the COPYING file, which should have been included
// with this distribution.
//  
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING file for more details.
//  
// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where
// to send such submissions.

#include <cstring>
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
  
  InitFieldBits();
  InitFields(__FrmHdr.GetFrameDef());
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

void ID3_Frame::InitFields(const ID3_FrameDef *info)
{
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
    __apFields[i]->__lFixedLength = info->aeFieldDefs[i].lFixedLength;
    __apFields[i]->__ucIOVersion  = info->aeFieldDefs[i].ucVersion;
    __apFields[i]->__ucIORevision = info->aeFieldDefs[i].ucRevision;
    __apFields[i]->__eControl     = info->aeFieldDefs[i].eControl;
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
    InitFields(__FrmHdr.GetFrameDef());
  }
}


ID3_FrameID ID3_Frame::GetID(void) const
{
  return __FrmHdr.GetFrameID();
}


void ID3_Frame::SetVersion(uchar ver, uchar rev)
{
  if (__FrmHdr.GetVersion() != ver || __FrmHdr.GetRevision() != rev)
  {
    __bHasChanged = true;
  }
  
  __FrmHdr.SetVersion(ver, rev);
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
    __apFields[i]->SetVersion(__FrmHdr.GetVersion(), __FrmHdr.GetRevision());
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
}

// $Log: frame.cpp,v $
// Revision 1.12  2000/01/04 15:42:49  eldamitri
// * include/id3/field.h:
// * include/id3/int28.h:
// * include/id3/misc_support.h:
// * include/id3/tag.h:
// * include/id3/types.h:
// * src/id3/dll_wrapper.cpp
// * src/id3/error.cpp
// * src/id3/field.cpp
// * src/id3/field_binary.cpp
// * src/id3/field_integer.cpp
// * src/id3/field_string_ascii.cpp
// * src/id3/field_string_unicode.cpp
// * src/id3/frame.cpp
// * src/id3/frame_parse.cpp
// * src/id3/frame_render.cpp
// * src/id3/header.cpp
// * src/id3/header_frame.cpp
// * src/id3/header_tag.cpp
// * src/id3/int28.cpp
// * src/id3/misc_support.cpp
// * src/id3/tag.cpp
// * src/id3/tag_file.cpp:
// * src/id3/tag_find.cpp:
// * src/id3/tag_parse.cpp:
// * src/id3/tag_parse_lyrics3.cpp:
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.11  1999/12/27 06:05:25  scott
// (ID3_Frame): Added data member initialization list.  Updated to reflect
// abstraction of field bits initialization.  Added new constructor which
// accepts a frame header to copy into the frame.
// (InitFieldBits): Added implementation.  Abstracted from the constuctor.
// (Clear): Updated to reflect addition of new header data member.
// Restructured method so it can be used in more situations.
// (InitFields): Added implementation. Abstracted from the SetID method.
// (SetID): Updated to reflect new method abstractions.
// (GetID, SetVersion, Size): Updated to reflect new frame header data
// member.
//
// Revision 1.10  1999/12/26 15:10:48  scott
// Minor reformatting.
//
// Revision 1.9  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.8  1999/12/09 03:32:17  scott
// (operator=): Added implementation.
//
// Revision 1.7  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:17:04  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
