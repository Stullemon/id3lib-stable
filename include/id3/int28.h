// $Id: int28.h,v 1.1 1999/12/01 17:16:07 scott Exp $
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

#ifndef ID3LIB_TYPES_28BITINT_H
#define ID3LIB_TYPES_28BITINT_H

#include <iostream.h>
#include <id3/types.h>

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
// Revision 1.1  1999/12/01 17:16:07  scott
// moved from src/id3 to include/id3
//
// Revision 1.5  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
