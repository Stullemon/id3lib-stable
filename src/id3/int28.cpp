// $Id: int28.cpp,v 1.11 2000/04/05 05:21:15 eldamitri Exp $

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

#include "int28.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

int28::int28(uint32 val)
{
  set(val);
}


int28::int28(uchar val[sizeof(uint32)])
{
  set(val);
}

void int28::set(uchar val[sizeof(uint32)])
{
  for (size_t i = 0; i < sizeof(uint32); i++)
  {
    __acValue[i] = val[i];
  }

  uchar bytes [4];
  bytes[0] = ((__acValue[0] >> 3) & MASK4);
  bytes[1] = ((__acValue[1] >> 2) & MASK5) | ((__acValue[0] & MASK3) << 5);
  bytes[2] = ((__acValue[2] >> 1) & MASK6) | ((__acValue[1] & MASK2) << 6);
  bytes[3] = ((__acValue[3] >> 0) & MASK7) | ((__acValue[2] & MASK1) << 7);

  __nValue = ParseNumber(bytes);
}

void int28::set(uint32 val)
{
  __nValue = val;
  for (size_t i = 0; i < sizeof(uint32); i++)
  {
    __acValue[sizeof(uint32) - 1 - i] = 
      (uchar) ((val >> (i * 7)) & MASK7) & MASK8;
  }
}

uint32 int28::get(void)
{
  return __nValue;
}

uchar int28::operator[](size_t posn)
{
  return __acValue[posn];
}

ostream& operator<<(ostream& out, int28& val)
{
  out.write(val.__acValue, sizeof(uint32));
  return out;
}


istream& operator>>(istream& in, int28& val)
{
  uchar temp [sizeof(uint32)];
  in.read(temp, sizeof(temp));
  val = temp;
  return in;
}

// $Log: int28.cpp,v $
// Revision 1.11  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.10  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.9  1999/12/26 15:11:39  scott
// (set): Now uses ParseNumber, defined in misc_support.  Restructured
// code.
//
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
