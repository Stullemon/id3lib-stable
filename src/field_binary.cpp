// $Id: field_binary.cpp,v 1.4 2000/04/27 15:51:26 eldamitri Exp $

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
    __sData = new uchar[newSize];
    if (NULL == __sData)
    {
      ID3_THROW(ID3E_NoMemory);
    }
    
    if (newData != NULL)
    {
      memcpy(__sData, newData, newSize);
    }
    else
    {
      memset(__sData, 0, newSize);
    }
    __ulSize = newSize;
    
    __eType = ID3FTY_BINARY;
    __bHasChanged = true;
  }
  
  return ;
}


void ID3_Field::Get(uchar *buffer, const luint buffLength)
{
  if (NULL == buffer)
  {
    ID3_THROW(ID3E_NoBuffer);
  }
    
  if (__sData != NULL && __ulSize > 0)
  {
    luint actualBytes = MIN(buffLength, __ulSize);
    
    memcpy(buffer, __sData, actualBytes);
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


void ID3_Field::ToFile(const char *info)
{
  if (NULL == info)
  {
    ID3_THROW(ID3E_NoData);
  }
    
  if ((__sData != NULL) && (__ulSize > 0))
  {
    FILE *temp;
    
    temp = fopen(info, "wb");
    if (temp != NULL)
    {
      fwrite(__sData, 1, __ulSize, temp);
      fclose(temp);
    }
  }
  
  return ;
}


size_t
ID3_Field::ParseBinary(const uchar *buffer, luint posn, size_t nSize)
{
  size_t bytesUsed = nSize - posn;
  
  if (__ulFixedLength > 0)
  {
    bytesUsed = MIN(__ulFixedLength, bytesUsed);
  }
    
  Set(&buffer[posn], bytesUsed);
  
  __bHasChanged = false;
  
  return bytesUsed;
}


luint 
ID3_Field::RenderBinary(uchar *buffer)
{
  luint bytesUsed = 0;
  
  bytesUsed = BinSize();
  memcpy(buffer, (uchar *) __sData, bytesUsed);
  
  __bHasChanged = false;
  
  return bytesUsed;
}



// $Log: field_binary.cpp,v $
// Revision 1.4  2000/04/27 15:51:26  eldamitri
// (Set): Now handles the case when null data is passed in (thanks John Adcock)
//
// Revision 1.3  2000/04/24 14:47:50  eldamitri
// __lFixedLength changed to __ulFixedLength.  A fixed length of 0, rather
// than -1, represents a variable-length field, since we should never have
// a fixed field length of 0.
//
// Revision 1.2  2000/04/18 22:10:20  eldamitri
// Moved field_binary.cpp from src/id3/ to src/
//
// Revision 1.13  2000/04/17 02:31:35  eldamitri
// Updated parameters of certain methods with const modifier to match
// declaration.
//
// Revision 1.12  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.11  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.10  1999/12/26 15:10:48  scott
// Minor reformatting.
//
// Revision 1.9  1999/12/17 16:13:03  scott
// Updated opening comment block.
//
// Revision 1.8  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/19 17:28:13  scott
// (Set): Updated interface to make parameters const.
// (FromFile): Updated interface to make parameters const.
// (ToFile): Updated interface to make parameters const.
// (ParseBinary): Updated interface to make parameters const.
//
// Revision 1.5  1999/11/15 20:15:50  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
