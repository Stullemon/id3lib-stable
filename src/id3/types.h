// $Id: types.h,v 1.6 1999/11/29 19:26:18 scott Exp $
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

#ifndef ID3LIB_TYPES_H
#define ID3LIB_TYPES_H

#include <wchar.h>
#include <id3/globals.h>

#ifdef __DLL
#define DLLEXPORT       __declspec ( dllexport )
#define CDLLEXPORT      extern "C" __declspec ( dllexport )
#else
#define DLLEXPORT
#define CDLLEXPORT
#endif

#define BS_SIZE (sizeof(luint)*8)
#define BS_SET(v,x)   ((v)[(x) / BS_SIZE] |=  (1 << ((x) % BS_SIZE)))
#define BS_CLEAR(v,x) ((v)[(x) / BS_SIZE] &= ~(1 << ((x) % BS_SIZE)))
#define BS_ISSET(v,x) ((v)[(x) / BS_SIZE] &   (1 << ((x) % BS_SIZE)))

#ifndef NULL
#define NULL (0L)
#endif

#ifndef MIN
inline lsint MIN(lsint x, lsint y)
{
  return x < y ? x : y;
}
#endif

#ifndef MAX
inline lsint MAX(lsint x, lsint y)
{
  return x > y ? x : y;
}
#endif

// include other abstract types here because they
// may depend on the types defined above
#include <id3/int28.h>

#endif

// $Log: types.h,v $
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/25 19:24:24  scott
// Removed typedefs, which are now included in the globas.h file.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
