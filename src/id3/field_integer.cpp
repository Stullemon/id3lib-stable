// $Id: field_integer.cpp,v 1.8 1999/12/01 18:00:59 scott Exp $
// 
// The authors have released ID3Lib as Public Domain (PD) and claim no
// copyright, patent or other intellectual property protection in this work.
// This means that it may be modified, redistributed and used in commercial
// and non-commercial software and hardware without restrictions.  ID3Lib is
// distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.
// 
// The ID3Lib authors encourage improvements and optimisations to be sent to
// the ID3Lib coordinator, currently Scott Haug (sth2@cs.wustl.edu).  Approved
// submissions may be altered, and will be included and released under these
// terms.

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


luint ID3_Field::Get(void)
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
      bytesUsed = MIN(__lFixedLength, bytesUsed);

    for(i = 0; i < bytesUsed; i++)
      temp |= (buffer[posn + i] << (((bytesUsed - i) - 1) * 8));
      
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
