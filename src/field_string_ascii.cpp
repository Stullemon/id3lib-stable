// $Id: field_string_ascii.cpp,v 1.5 2000/05/03 03:02:56 eldamitri Exp $

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
#include <stdlib.h>
#include "field.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

ID3_Field& ID3_Field::operator= (const char *string)
{
  Set(string);
  
  return *this;
}


// the ::Set() function for ASCII

void ID3_Field::Set(const char *sString)
{
  if (sString != NULL)
  {
    Clear();
    size_t nStrLen = (0 == __length) ? strlen(sString) : __length;
    unicode_t *temp = new unicode_t[nStrLen + 1];
    if (NULL == temp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    mbstoucs(temp, sString, nStrLen + 1);

    Set(temp);
    delete [] temp;
      
    __type = ID3FTY_ASCIISTRING;
  }
  
  return ;
}


// the ::Get() function for ASCII

luint ID3_Field::Get(char *buffer, const luint maxLength, const luint itemNum)
{
  luint bytesUsed = 0;
  unicode_t *temp;
  char *ascii;
  
  temp = new unicode_t[maxLength];
  if (NULL == temp)
  {
    delete [] temp;
    ID3_THROW(ID3E_NoMemory);
  }

  luint len = Get(temp, maxLength, itemNum);

  ascii = new char[len + 1];
  if (NULL == ascii)
  {
    delete [] ascii;
    ID3_THROW(ID3E_NoMemory);
  }

  luint length;
        
  ucstombs(ascii, temp, len + 1);
        
  length = MIN(strlen(ascii), maxLength);
        
  strncpy(buffer, ascii, length);
  buffer[length] = '\0';
  bytesUsed = length;
        
  delete [] ascii;
    
  delete [] temp;
    
  return bytesUsed;
}


void ID3_Field::Add(const char *sString)
{
  if (sString)
  {
    unicode_t *temp;
    
    temp = new unicode_t[strlen(sString) + 1];
    if (NULL == temp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    mbstoucs(temp, sString, strlen(sString) + 1);
    Add(temp);
    delete [] temp;
    
    __type = ID3FTY_ASCIISTRING;
  }
  
  return ;
}


size_t 
ID3_Field::ParseASCIIString(const uchar *buffer, luint posn, size_t nSize)
{
  size_t bytesUsed = 0;
  char *temp = NULL;
  
  if (__length > 0)
  {
    // The string is of fixed length
    bytesUsed = __length;
  }
  else if (!(__flags & ID3FF_NULL) || (__flags & ID3FF_NULLDIVIDE))
  {
    // If the string isn't null-terminated or if it is null divided, we're
    // assured this is the last field of of the frame, and we can claim the
    // remaining bytes for ourselves
    bytesUsed = nSize - posn;
  }
  else
  {
    while ((posn + bytesUsed) < nSize && buffer[posn + bytesUsed] != '\0')
    {
      bytesUsed++;
    }
  }

  if (0 == bytesUsed)
  {
    Set("");
  }
  // This check needs to come before the check for ID3FF_NULL
  else if (__flags & ID3FF_NULLDIVIDE)
  {
    char *sBuffer = (char *) &buffer[posn];
    for (size_t nIndex = 0; nIndex < bytesUsed; 
         nIndex += strlen(&sBuffer[nIndex]) + 1)
    {
      Add(&sBuffer[nIndex]);
    }
  }
  // This check needs to come after the check for ID3FF_NULLDIVIDE
  else if (__flags & ID3FF_NULL)
  {
    char *sBuffer = (char *) &buffer[posn];
    Set(sBuffer);
  }
  else
  {
    // Sanity check our indices and sizes before we start copying memory
    if ((bytesUsed > nSize) || (posn + bytesUsed > nSize))
    {
      ID3_THROW_DESC(ID3E_BadData, "field information invalid");
    }

    temp = new char[bytesUsed + 1];
    if (NULL == temp)
    {
      ID3_THROW(ID3E_NoMemory);
    }
    
    memcpy(temp, &buffer[posn], bytesUsed);
    temp[bytesUsed] = '\0';
    Set(temp);
      
    delete [] temp;
  }
  
  if (__flags & ID3FF_NULL && !(__flags & ID3FF_NULLDIVIDE))
  {
    bytesUsed++;
  }
    
  __changed = false;
  
  return bytesUsed;
}


luint ID3_Field::RenderASCIIString(uchar *buffer)
{
  luint nChars = BinSize();

  if ((NULL != __data) && (nChars > 0))
  {
    ucstombs((char *) buffer, (unicode_t *) __data, nChars);
      
    // now we convert the internal dividers to what they are supposed to be
    for (luint nIndex = 0; nIndex < nChars; nIndex++)
    {
      if ('\1' == buffer[nIndex])
      {
        char sub = '/';
          
        if (__flags & ID3FF_NULLDIVIDE)
        {
          sub = '\0';
        }
        buffer[nIndex] = sub;
      }
    }
  }
  
  if ((1 == nChars) && (__flags & ID3FF_NULL))
  {
    buffer[0] = '\0';
  }
    
  __changed = false;
  
  return nChars;
}
