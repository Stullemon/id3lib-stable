// $Id: field_binary.cpp,v 1.7 2000/05/08 04:07:39 eldamitri Exp $

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


#include <stdio.h>
#include <memory.h>
#include "field.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

void ID3_Field::Set(const uchar *newData, const luint newSize)
{
  Clear();
  
  if (newSize > 0)
  {
    __data = new uchar[newSize];
    if (NULL == __data)
    {
      ID3_THROW(ID3E_NoMemory);
    }
    
    if (newData != NULL)
    {
      memcpy(__data, newData, newSize);
    }
    else
    {
      memset(__data, 0, newSize);
    }
    __size = newSize;
    
    __type = ID3FTY_BINARY;
    __changed = true;
  }
  
  return ;
}


void ID3_Field::Get(uchar *buffer, const luint buffLength)
{
  if (NULL == buffer)
  {
    ID3_THROW(ID3E_NoBuffer);
  }
    
  if (__data != NULL && __size > 0)
  {
    luint actualBytes = MIN(buffLength, __size);
    
    memcpy(buffer, __data, actualBytes);
  }
  
  return ;
}


void ID3_Field::FromFile(const char *info)
{
  FILE *temp;
  luint fileSize;
  uchar *buffer;
  
  if (NULL == info)
  {
    ID3_THROW(ID3E_NoData);
  }
    
  temp = fopen(info, "rb");
  if (temp != NULL)
  {
    fseek(temp, 0, SEEK_END);
    fileSize = ftell(temp);
    fseek(temp, 0, SEEK_SET);
    
    buffer = new uchar[fileSize];
    if (buffer != NULL)
    {
      fread(buffer, 1, fileSize, temp);
      
      Set(buffer, fileSize);
      
      delete [] buffer;
    }
    
    fclose(temp);
  }
  
  return ;
}


void ID3_Field::ToFile(const char *info) const
{
  if (NULL == info)
  {
    ID3_THROW(ID3E_NoData);
  }
    
  if ((__data != NULL) && (__size > 0))
  {
    FILE *temp;
    
    temp = fopen(info, "wb");
    if (temp != NULL)
    {
      fwrite(__data, 1, __size, temp);
      fclose(temp);
    }
  }
  
  return ;
}


size_t
ID3_Field::ParseBinary(const uchar *buffer, luint posn, size_t nSize)
{
  size_t bytesUsed = nSize - posn;
  
  if (__length > 0)
  {
    bytesUsed = MIN(__length, bytesUsed);
  }
    
  Set(&buffer[posn], bytesUsed);
  
  __changed = false;
  
  return bytesUsed;
}


luint 
ID3_Field::RenderBinary(uchar *buffer)
{
  luint bytesUsed = 0;
  
  bytesUsed = BinSize();
  memcpy(buffer, (uchar *) __data, bytesUsed);
  
  __changed = false;
  
  return bytesUsed;
}
