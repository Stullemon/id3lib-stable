// $Id: frame.cpp,v 1.26 2000/09/21 23:16:41 eldamitri Exp $

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
#include "field_impl.h"
#include "frame_def.h"
#include "field_def.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

/** \class ID3_Frame
 ** \brief The representative class of an id3v2 frame.
 ** 
 ** id3lib defines frames in a funny way.  Using some nice c++ conventions,
 ** ID3_Frame class objects appear to be quite polymorphic; that is, they can
 ** take on many forms.  The same ID3_Frame class provides the facilities for
 ** the implementation of a complex APIC frame and for a simple text frame.
 ** 
 ** @author Dirk Mahoney
 ** @version $Id: frame.cpp,v 1.26 2000/09/21 23:16:41 eldamitri Exp $
 ** @see ID3_Tag
 ** @see ID3_Field
 ** @see ID3_Err
 **/

/** Default constructor; accepts as a default parameter the type of frame
 ** to create.
 ** 
 ** The parameter which will internally set the frame's structure.  See
 ** SetID() for more details.
 **     
 ** @param id The type of frame to create
 ** @see ID3_FrameID
 ** @see SetID
 **/
ID3_Frame::ID3_Frame(ID3_FrameID id)
  : _changed(false),
    _field_bitset(NULL),
    _num_fields(0),
    _fields(NULL),
    _encryption_id('\0'),
    _grouping_id('\0'),
    _bad_parse(false)
{
  this->SetSpec(ID3V2_LATEST);
  this->_InitFieldBits();
  this->SetID(id);
}

ID3_Frame::ID3_Frame(const ID3_FrameHeader &hdr)
  : _changed(false),
    _field_bitset(NULL),
    _num_fields(0),
    _fields(NULL),
    _hdr(hdr),
    _encryption_id('\0'),
    _grouping_id('\0'),
    _bad_parse(false)
{
  this->_InitFieldBits();
  this->_InitFields();
}

ID3_Frame::ID3_Frame(const ID3_Frame& frame)
  : _changed(false),
    _field_bitset(NULL),
    _num_fields(0),
    _fields(NULL),
    _encryption_id('\0'),
    _grouping_id('\0'),
    _bad_parse(false)
{
  this->_InitFieldBits();
  *this = frame;
}

void ID3_Frame::_InitFieldBits()
{
  size_t lWordsForFields =
    (((uint32) ID3FN_LASTFIELDID) - 1) / (sizeof(uint32) * 8);
  
  if ((((uint32) ID3FN_LASTFIELDID) - 1) % (sizeof(uint32) * 8) != 0)
  {
    lWordsForFields++;
  }
   
  if (_field_bitset != NULL)
  {
    delete [] _field_bitset;
  }
  _field_bitset = new uint32[lWordsForFields];
  if (NULL == _field_bitset)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  for (index_t i = 0; i < lWordsForFields; i++)
  {
    _field_bitset[i] = 0;
  }
}

ID3_Frame::~ID3_Frame()
{
  Clear();
  
  if (_field_bitset != NULL)
  {
    delete [] _field_bitset;
  }
}

bool ID3_Frame::_ClearFields()
{
  if (!_num_fields || !_fields)
  {
    return false;
  }
  for (index_t fi = 0; fi < _num_fields; fi++)
  {
    delete dynamic_cast<ID3_FieldImpl*>(_fields[fi]);
  }
  
  delete [] _fields;
  _fields = NULL;
    
  size_t lWordsForFields =
    (((uint32) ID3FN_LASTFIELDID) - 1) / (sizeof(uint32) * 8);
  
  if ((((uint32) ID3FN_LASTFIELDID) - 1) % (sizeof(uint32) * 8) != 0)
  {
    lWordsForFields++;
  }

  for (index_t i = 0; i < lWordsForFields; i++)
  {
    _field_bitset[i] = 0;
  }
   
  _changed = true;
  return true;
}

/** Clears the frame of all data and resets the frame such that it can take
 ** on the form of any id3v2 frame that id3lib supports.
 ** 
 ** @see ID3_Tag::Clear
 **/
void ID3_Frame::Clear()
{
  this->_ClearFields();
  _hdr.Clear();
  _encryption_id   = '\0';
  _grouping_id     = '\0';
  _num_fields      = 0;
  _fields          = NULL;
}

void ID3_Frame::_InitFields()
{
  const ID3_FrameDef* info = _hdr.GetFrameDef();
  if (NULL == info)
  {
    ID3_THROW(ID3E_InvalidFrameID);
  }
  
  _num_fields = 0;
  
  while (info->aeFieldDefs[_num_fields]._id != ID3FN_NOFIELD)
  {
    _num_fields++;
  }
  
  _fields = new ID3_Field * [_num_fields];
  if (NULL == _fields)
  {
    ID3_THROW(ID3E_NoMemory);
  }
  
  for (index_t i = 0; i < _num_fields; i++)
  {
    _fields[i] = new ID3_FieldImpl(info->aeFieldDefs[i]);
    if (NULL == _fields[i])
    {
      ID3_THROW(ID3E_NoMemory);
    }
    
    // tell the frame that this field is present
    BS_SET(_field_bitset, _fields[i]->GetID());
  }
  
  _changed = true;
}

/** Establishes the internal structure of an ID3_Frame object so
 ** that it represents the id3v2 frame indicated by the parameter
 ** 
 ** Given an ID3_FrameID (a list of which is found in &lt;id3/field.h&gt;),
 ** SetID() will structure the object according to the
 ** frame you wish to implement.
 ** 
 ** Either using this call or via the constructor, this must be the first
 ** command performed on an ID3_Frame object.  
 ** 
 ** \code
 **   myFrame.SetID(ID3FID_TITLE);
 ** \endcode
 ** 
 ** @param id The type of frame this frame should be set to
 ** @see ID3_FrameID
 **/
bool ID3_Frame::SetID(ID3_FrameID id)
{
  bool changed = (this->GetID() != id);
  if (changed)
  {
    this->_SetID(id);
    _changed = true;
  }
  return changed;
}

bool ID3_Frame::_SetID(ID3_FrameID id)
{
  bool changed = this->_ClearFields();
  changed = _hdr.SetFrameID(id) || changed;
  this->_InitFields();
  return changed;
}

/** \fn ID3_FrameID GetID() const
 ** \brief Returns the type of frame that the object represents.
 ** 
 ** Useful in conjunction with ID3_Tag::Find() method
 ** 
 ** @returns The type, or id, of the frame
 ** @see ID3_Tag::Find
 **/

bool ID3_Frame::SetSpec(ID3_V2Spec spec)
{
  return _hdr.SetSpec(spec);
}

ID3_V2Spec ID3_Frame::GetSpec() const
{
  return _hdr.GetSpec();
}

/** Returns a reference to the frame's internal field indicated by the
 ** parameter.
 ** 
 ** A list of fields that are in given frames appears in <id3/field.cpp>.  This
 ** method returns a reference to the field in question so that the result can
 ** be used as though it were a field itself.
 **
 ** \code
 **   ID3_TextEnc enc;
 **   enc = (ID3_TextEnc) myFrame.Field(ID3FN_TEXTENC).Get();
 ** \endcode
 ** 
 ** @param name The name of the field to be retrieved
 ** @returns A reference to the desired field
 **/
ID3_Field& ID3_Frame::Field(ID3_FieldID fieldName) const
{
  ID3_Field* field = NULL;
  if (BS_ISSET(_field_bitset, fieldName))
  {
    for (size_t num = 0; num < _num_fields; num++)
    {
      if (_fields[num]->GetID() == fieldName)
      {
        field = _fields[num];
      }
    }
  }
  
  if (!field)
  {
    ID3_THROW(ID3E_FieldNotFound);
  }
  
  return *field;
}

size_t ID3_Frame::Size()
{
  size_t bytesUsed = _hdr.Size();
  
  if (this->_GetEncryptionID())
  {
    bytesUsed++;
  }
    
  if (this->_GetGroupingID())
  {
    bytesUsed++;
  }
    
  ID3_TextEnc enc = ID3TE_ASCII;
  for (ID3_Field** fi = _fields; fi != _fields + _num_fields; fi++)
  {
    if (*fi && (*fi)->InScope(this->GetSpec()))
    {
      if ((*fi)->GetID() == ID3FN_TEXTENC)
      {
        enc = (ID3_TextEnc) (*fi)->Get();
      }
      else
      {
        (*fi)->SetEncoding(enc);
      }
      bytesUsed += (*fi)->BinSize();
    }
  }
  
  return bytesUsed;
}


bool ID3_Frame::HasChanged() const
{
  bool changed = _changed;
  
  for (ID3_Field** fi = _fields; !changed && fi != _fields + _num_fields; fi++)
  {
    if (*fi && (*fi)->InScope(this->GetSpec()))
    {
      changed = (*fi)->HasChanged();
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
    for (size_t nIndex = 0; nIndex < _num_fields; nIndex++)
    {
      if (rFrame._fields[nIndex] != NULL)
      {
        *(_fields[nIndex]) = *(rFrame._fields[nIndex]);
      }
    }
    this->_SetEncryptionID(rFrame._GetEncryptionID());
    this->_SetGroupingID(rFrame._GetGroupingID());
    this->SetCompression(rFrame.GetCompression());
    this->SetSpec(rFrame.GetSpec());
    _bad_parse = false;
    _changed = false;
  }
  return *this;
}

const char* ID3_Frame::GetDescription(ID3_FrameID id)
{
  ID3_FrameDef* myFrameDef = ID3_FindFrameDef(id);
  if (myFrameDef != NULL)
  {
    return myFrameDef->sDescription;
  }
  return NULL;
}

const char* ID3_Frame::GetDescription() const
{
  const ID3_FrameDef* def = _hdr.GetFrameDef();
  if (def)
  {
    return def->sDescription;
  }
  return NULL;
}

