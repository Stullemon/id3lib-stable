// $Id: frame.cpp,v 1.30 2000/10/15 06:39:26 eldamitri Exp $

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

#include "frame.h"
#include "frame_impl.h"
#include "readers.h"

ID3_Frame::ID3_Frame(ID3_FrameID id)
  : _impl(new ID3_FrameImpl(id))
{
}

ID3_Frame::ID3_Frame(const ID3_Frame& frame)
  : _impl(new ID3_FrameImpl(frame))
{
}

ID3_Frame::~ID3_Frame()
{
  delete _impl;
}

void ID3_Frame::Clear()
{
  _impl->Clear();
}

ID3_FrameID ID3_Frame::GetID() const
{
  return _impl->GetID();
}

bool ID3_Frame::SetID(ID3_FrameID id)
{
  return _impl->SetID(id);
}

bool ID3_Frame::SetSpec(ID3_V2Spec spec)
{
  return _impl->SetSpec(spec);
}

ID3_V2Spec ID3_Frame::GetSpec() const
{
  return _impl->GetSpec();
}

ID3_Field& ID3_Frame::Field(ID3_FieldID fieldName) const
{
  return *this->GetField(fieldName);
}

ID3_Field* ID3_Frame::GetField(ID3_FieldID fieldName) const
{
  return _impl->GetField(fieldName);
}

size_t ID3_Frame::NumFields() const
{
  return _impl->NumFields();
}

ID3_Field* ID3_Frame::GetFieldNum(index_t index) const
{
  return _impl->GetFieldNum(index);
}

size_t ID3_Frame::Size()
{
  return _impl->Size();
}


bool ID3_Frame::HasChanged() const
{
  return _impl->HasChanged();
}

ID3_Frame& ID3_Frame::operator=( const ID3_Frame &rFrame )
{
  if (this != &rFrame)
  {
    *_impl = rFrame;
  }
  return *this;
}

const char* ID3_Frame::GetDescription(ID3_FrameID id)
{
  return ID3_FrameImpl::GetDescription(id);
}

const char* ID3_Frame::GetDescription() const
{
  return _impl->GetDescription();
}

const char* ID3_Frame::GetTextID() const
{
  return _impl->GetTextID();
}

bool ID3_Frame::Parse(ID3_Reader& reader) 
{
  return _impl->Parse(reader);
}

void ID3_Frame::Render(ID3_Writer& writer) const
{
  _impl->Render(writer);
}

bool ID3_Frame::Contains(ID3_FieldID id) const
{
  return _impl->Contains(id);
}

bool ID3_Frame::SetCompression(bool b)
{
  return _impl->SetCompression(b);
}

bool ID3_Frame::GetCompression() const
{
  return _impl->GetCompression();
}

size_t ID3_Frame::GetDataSize() const
{
  return _impl->GetDataSize();
}

bool ID3_Frame::SetEncryptionID(uchar id)
{
  return _impl->SetEncryptionID(id);
}

uchar ID3_Frame::GetEncryptionID() const
{
  return _impl->GetEncryptionID();
}

bool ID3_Frame::SetGroupingID(uchar id)
{
  return _impl->SetGroupingID(id);
}

uchar ID3_Frame::GetGroupingID() const
{
  return _impl->GetGroupingID();
}

