// $Id: field_integer.cpp,v 1.10 1999/12/17 16:13:04 scott Exp $
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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include "field.h"

ID3_Field& ID3_Field::operator=(luint newData)
{
  Set(newData);
  
  return *this;
}


void ID3_Field::Set(luint newData)
{
  Clear();
  
  __sData = (uchar *) newData;
  __ulSize = sizeof(luint);
  __eType = ID3FTY_INTEGER;
  __bHasChanged = true;
  
  return ;
}


luint ID3_Field::Get(void) const
{
  return (luint) __sData;
}


luint ID3_Field::ParseInteger(const uchar *buffer, const luint posn, const luint buffSize)
{
  luint bytesUsed = 0;

  if (buffer != NULL && buffSize > 0)
  {
    luint i;
    luint temp = 0;
    
    bytesUsed = 4;
    
    if (__lFixedLength != -1)
    {
      bytesUsed = MIN(__lFixedLength, bytesUsed);
    }

    for (i = 0; i < bytesUsed; i++)
    {
      temp |= (buffer[posn + i] << (((bytesUsed - i) - 1) * 8));
    }
      
    Set(temp);
    __bHasChanged = false;
  }
  
  return bytesUsed;
}


luint ID3_Field::RenderInteger(uchar *buffer)
{
  luint bytesUsed = 0;
  luint length = BinSize();
  
  for(luint i = 0; i < length; i++)
    buffer[i] = (uchar)((((luint) __sData) >> (((length - i) - 1) * 8)) & 0xFF);
    
  bytesUsed = length;
  __bHasChanged = false;
  
  return bytesUsed;
}

// $Log: field_integer.cpp,v $
// Revision 1.10  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.9  1999/12/09 03:32:02  scott
// (Get): Added const qualifier.
//
// Revision 1.8  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/19 17:29:18  scott
// (ParseInteger): Updated interface to make parameters const.
//
// Revision 1.5  1999/11/15 20:16:06  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
