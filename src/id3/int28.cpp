// $Id: int28.cpp,v 1.8 1999/12/17 16:13:04 scott Exp $
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

#include "int28.h"

int28::int28(luint val)
{
  set(val);
}


int28::int28(uchar *val)
{
  for (luint i = 0; i < sizeof(luint); i++)
    value[i] = val[i];
}


void int28::set(luint val)
{
  for (luint i = 0; i < sizeof(luint); i++)
    value[sizeof(luint) - 1 - i] = (uchar)((val >> (i * 7)) & 127) & 0xFF;
    
  return ;
}


luint int28::get(void)
{
  luint newSize = 0L;
  uchar bytes [4];
  
  bytes[3] = value[3] | ((value[2] & 1) << 7);
  bytes[2] = ((value[2] >> 1) & 63) | ((value[1] & 3) << 6);
  bytes[1] = ((value[1] >> 2) & 31) | ((value[0] & 7) << 5);
  bytes[0] = ((value[0] >> 3) & 15);
  
  newSize = bytes[3] | ((luint) bytes[2] << 8) |
    ((luint) bytes[1] << 16) | ((luint) bytes[0] << 24);
  
  return newSize;
}


uchar int28::operator[](luint posn)
{
  return value[posn];
}


ostream& operator<<(ostream& out, int28& val)
{
  out.write(val.value, sizeof(val));
  
  return out;
}


istream& operator>>(istream& in, int28& val)
{
  uchar temp [sizeof(luint)];
  
  in.read(temp, sizeof(temp));
  
  val = temp;
  
  return in;
}

// $Log: int28.cpp,v $
// Revision 1.8  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.7  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:19:47  scott
// Added include for config.h. Changed some ints to luints to avoid
// compiler warnings.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
