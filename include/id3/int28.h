// $Id: int28.h,v 1.3 1999/12/17 16:05:02 scott Exp $
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

#ifndef ID3LIB_TYPES_28BITINT_H
#define ID3LIB_TYPES_28BITINT_H

#include <iostream.h>
#include "types.h"

class int28
{
public:
  int28(luint val = 0);
  int28(uchar *val);
  
  uchar operator[](luint posn);
  friend ostream &operator<<(ostream& out, int28& val);
  luint get(void);
  
  // *** PRIVATE INTERNAL DATA - DO NOT USE *** PRIVATE INTERNAL DATA - DO NOT USE ***
  
protected:
  void set(luint val);
  uchar value[sizeof(luint)]; // the integer stored as a uchar array
}
;

ostream & operator<<(ostream& out, int28& val);
istream & operator>>(istream& in, int28& val);

#endif

// $Log: int28.h,v $
// Revision 1.3  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:07  scott
// moved from src/id3 to include/id3
//
// Revision 1.5  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
