// $Id: frame_impl.cpp,v 1.6 2000/10/21 15:44:59 eldamitri Exp $

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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "tag.h"
#include "frame_impl.h"
#include "field_impl.h"
#include "frame_def.h"
#include "field_def.h"

/** \class ID3_FrameImpl
 ** \brief The representative class of an id3v2 frame.
 ** 
 ** id3lib defines frames in a funny way.  Using some nice c++ conventions,
 ** ID3_FrameImpl class objects appear to be quite polymorphic; that is, they
 ** can take on many forms.  The same ID3_FrameImpl class provides the
 ** facilities for the implementation of a complex APIC frame and for a simple
 ** text frame.
 ** 
 ** @author Dirk Mahoney
 ** @version $Id: frame_impl.cpp,v 1.6 2000/10/21 15:44:59 eldamitri Exp $
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
ID3_FrameImpl::ID3_FrameImpl(ID3_FrameID id)
  : _changed(false),
    _bitset(),
    _fields(),
    _encryption_id('\0'),
    _grouping_id('\0')
{
  this->SetSpec(ID3V2_LATEST);
  this->SetID(id);
}

ID3_FrameImpl::ID3_FrameImpl(const ID3_FrameHeader &hdr)
  : _changed(false),
    _bitset(),
    _fields(),
    _hdr(hdr),
    _encryption_id('\0'),
    _grouping_id('\0')
{
  this->_InitFields();
}

ID3_FrameImpl::ID3_FrameImpl(const ID3_Frame& frame)
  : _changed(false),
    _bitset(),
    _fields(),
    _encryption_id('\0'),
    _grouping_id('\0')
{
  *this = frame;
}

ID3_FrameImpl::~ID3_FrameImpl()
{
  Clear();
}

bool ID3_FrameImpl::_ClearFields()
{
  for (iterator fi = _fields.begin(); fi != _fields.end(); ++fi)
  {
    delete (ID3_FieldImpl*) *fi;
  }

  _fields.clear();
  _bitset.reset();

  _changed = true;
  return true;
}

/** Clears the frame of all data and resets the frame such that it can take
 ** on the form of any id3v2 frame that id3lib supports.
 ** 
 ** @see ID3_Tag::Clear
 **/
void ID3_FrameImpl::Clear()
{
  this->_ClearFields();
  _hdr.Clear();
  _encryption_id   = '\0';
  _grouping_id     = '\0';
}

void ID3_FrameImpl::_InitFields()
{
  const ID3_FrameDef* info = _hdr.GetFrameDef();
  if (NULL == info)
  {
    // log this
    ID3_Field* fld = new ID3_FieldImpl(ID3_FieldDef::DEFAULT[0]);
    _fields.push_back(fld);
    _bitset.set(fld->GetID());
  }
  else
  {
    
    for (size_t i = 0; info->aeFieldDefs[i]._id != ID3FN_NOFIELD; ++i)
    {
      ID3_Field* fld = new ID3_FieldImpl(info->aeFieldDefs[i]);
      _fields.push_back(fld);
      _bitset.set(fld->GetID());
    }
    
    _changed = true;
  }
}

/** Establishes the internal structure of an ID3_FrameImpl object so
 ** that it represents the id3v2 frame indicated by the parameter
 ** 
 ** Given an ID3_FrameID (a list of which is found in &lt;id3/field.h&gt;),
 ** SetID() will structure the object according to the
 ** frame you wish to implement.
 ** 
 ** Either using this call or via the constructor, this must be the first
 ** command performed on an ID3_FrameImpl object.  
 ** 
 ** \code
 **   myFrame.SetID(ID3FID_TITLE);
 ** \endcode
 ** 
 ** @param id The type of frame this frame should be set to
 ** @see ID3_FrameID
 **/
bool ID3_FrameImpl::SetID(ID3_FrameID id)
{
  bool changed = (this->GetID() != id);
  if (changed)
  {
    this->_SetID(id);
    _changed = true;
  }
  return changed;
}

bool ID3_FrameImpl::_SetID(ID3_FrameID id)
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

bool ID3_FrameImpl::SetSpec(ID3_V2Spec spec)
{
  return _hdr.SetSpec(spec);
}

ID3_V2Spec ID3_FrameImpl::GetSpec() const
{
  return _hdr.GetSpec();
}

/** Returns a pointer to the frame's internal field indicated by the
 ** parameter.
 **
 ** \code
 **   ID3_TextEnc enc;
 **   enc = (ID3_TextEnc) myFrame.GetField(ID3FN_TEXTENC)->Get();
 ** \endcode
 ** 
 ** @param name The name of the field to be retrieved
 ** @returns A reference to the desired field
 **/
ID3_Field* ID3_FrameImpl::GetField(ID3_FieldID fieldName) const
{
  ID3_Field* field = NULL;
  if (this->Contains(fieldName))
  {
    for (const_iterator fi = _fields.begin(); fi != _fields.end(); ++fi)
    {
      if ((*fi)->GetID() == fieldName)
      {
        field = *fi;
        break;
      }
    }
  }
  return field;
}

size_t ID3_FrameImpl::NumFields() const
{
  return _fields.size();
}

ID3_Field* ID3_FrameImpl::GetFieldNum(index_t index) const
{
  ID3_Field* field = NULL;
  if (index < this->NumFields())
  {
    field = _fields[index];
  }
  return field;
}

size_t ID3_FrameImpl::Size()
{
  size_t bytesUsed = _hdr.Size();
  
  if (this->GetEncryptionID())
  {
    bytesUsed++;
  }
    
  if (this->GetGroupingID())
  {
    bytesUsed++;
  }
    
  ID3_TextEnc enc = ID3TE_ASCII;
  for (iterator fi = _fields.begin(); fi != _fields.end(); ++fi)
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


bool ID3_FrameImpl::HasChanged() const
{
  bool changed = _changed;
  
  for (const_iterator fi = _fields.begin(); fi != _fields.end(); ++fi)
  {
    if (*fi && (*fi)->InScope(this->GetSpec()))
    {
      changed = (*fi)->HasChanged();
    }
  }
  
  return changed;
}

ID3_FrameImpl &
ID3_FrameImpl::operator=( const ID3_Frame &rFrame )
{
  ID3_FrameID eID = rFrame.GetID();
  this->SetID(eID);
  for (size_t nIndex = 0; nIndex < this->NumFields(); nIndex++)
  {
    ID3_Field
      *thisFld = this->GetFieldNum(nIndex),
      *thatFld = rFrame.GetFieldNum(nIndex);
    if (thisFld != NULL && thatFld != NULL)
    {
      *thisFld = *thatFld;
    }
  }
  this->SetEncryptionID(rFrame.GetEncryptionID());
  this->SetGroupingID(rFrame.GetGroupingID());
  this->SetCompression(rFrame.GetCompression());
  this->SetSpec(rFrame.GetSpec());
  _changed = false;
  
  return *this;
}

const char* ID3_FrameImpl::GetDescription(ID3_FrameID id)
{
  ID3_FrameDef* myFrameDef = ID3_FindFrameDef(id);
  if (myFrameDef != NULL)
  {
    return myFrameDef->sDescription;
  }
  return NULL;
}

const char* ID3_FrameImpl::GetDescription() const
{
  const ID3_FrameDef* def = _hdr.GetFrameDef();
  if (def)
  {
    return def->sDescription;
  }
  return NULL;
}

