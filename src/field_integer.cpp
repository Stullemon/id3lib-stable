// $Id: field_integer.cpp,v 1.11 2000/09/14 21:58:39 eldamitri Exp $

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

#include "field.h"
#include "utils.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

/** \fn ID3_Field& ID3_Field::operator=(uint32 val)
 ** \brief A shortcut for the Set method.
 **
 ** \code
 **   myFrame.Field(ID3FN_PICTURETYPE) = 0x0B;
 ** \endcode
 ** 
 ** \param val The data to assign to this field
 ** \sa Set(uint32)
 **/

/** \brief Sets the value of the field to the specified integer.
 ** \param data The data to assign to this field
 **/
void ID3_Field::Set(uint32 val)
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
 **   uint32 picType = myFrame.Field(ID3FN_PICTURETYPE).Get();
 ** \endcode
 **
 ** \return The value of the integer field
 **/

size_t ID3_Field::ParseInteger(const uchar *buffer, size_t nSize)
{
  size_t nBytes = 0;

  if (buffer != NULL && nSize > 0)
  {
    this->Clear();
    size_t fixed = this->Size();
    nBytes = (fixed > 0) ? MIN(nSize, fixed) : nSize;
    this->Set(ParseNumber(buffer, nBytes));
    _changed = false;
  }
  
  return nBytes;
}


size_t ID3_Field::RenderInteger(uchar *buffer) const
{
  size_t bytesUsed = RenderNumber(buffer, _integer, this->Size());
  _changed = false;
  return bytesUsed;
}
