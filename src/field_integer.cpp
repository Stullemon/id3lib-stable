// $Id: field_integer.cpp,v 1.16 2000/10/09 04:25:49 eldamitri Exp $

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

#include "debug.h"
#include "field_impl.h"
#include "utils.h"
#include "reader_decorators.h"
#include "writer_decorators.h"

using namespace dami;

/** \fn ID3_Field& ID3_Field::operator=(uint32 val)
 ** \brief A shortcut for the Set method.
 **
 ** \code
 **   myFrame.GetField(ID3FN_PICTURETYPE)->= 0x0B;
 ** \endcode
 ** 
 ** \param val The data to assign to this field
 ** \sa Set(uint32)
 **/

/** \brief Sets the value of the field to the specified integer.
 ** \param data The data to assign to this field
 **/
void ID3_FieldImpl::Set(uint32 val)
{
  if (this->GetType() == ID3FTY_INTEGER)
  {
    this->Clear();
    
    _integer = val;
    _changed = true;
  }
}

/** \fn uint32 ID3_Field::Get() const
 ** \brief Returns the value of the integer field.
 ** 
 ** \code
 **   uint32 picType = myFrame.GetField(ID3FN_PICTURETYPE)->Get();
 ** \endcode
 **
 ** \return The value of the integer field
 **/

/** \fn uint32 ID3_Field::Get() const
 ** \brief Returns the value of the integer field.
 ** 
 ** \code
 **   uint32 picType = myFrame.GetField(ID3FN_PICTURETYPE)->Get();
 ** \endcode
 **
 ** \return The value of the integer field
 **/

bool ID3_FieldImpl::ParseInteger(ID3_Reader& reader)
{
  ID3D_NOTICE( "ID3_FieldImpl::ParseInteger(): beg = " << reader.getBeg() );
  ID3D_NOTICE( "ID3_FieldImpl::ParseInteger(): cur = " << reader.getCur() );
  ID3D_NOTICE( "ID3_FieldImpl::ParseInteger(): end = " << reader.getEnd() );
  bool success = false;
  if (!reader.atEnd())
  {
    this->Clear();
    size_t fixed = this->Size();
    size_t nBytes = (fixed > 0) ? fixed : sizeof(uint32);
    io::BinaryNumberReader nr(reader);
    this->Set(nr.readNumber(nBytes));
    _changed = false;
    success = true;
  }
  return success;
}

void ID3_FieldImpl::RenderInteger(ID3_Writer& writer) const
{
  io::BinaryNumberWriter bnw(writer);
  bnw.writeNumber(_integer, this->Size());
}
