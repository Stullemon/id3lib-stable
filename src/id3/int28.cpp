// $Id: int28.cpp,v 1.6 1999/11/29 19:26:18 scott Exp $
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

#include <id3/int28.h>

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
