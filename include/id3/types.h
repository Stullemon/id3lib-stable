// $Id: types.h,v 1.3 1999/12/17 16:05:02 scott Exp $
// 
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

#ifndef ID3LIB_TYPES_H
#define ID3LIB_TYPES_H

#include "globals.h"

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
#include "int28.h"

#endif

// $Log: types.h,v $
// Revision 1.3  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:11  scott
// moved from src/id3 to include/id3
//
// Revision 1.7  1999/11/30 20:14:22  scott
// Removed include for wchar.h
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/25 19:24:24  scott
// Removed typedefs, which are now included in the globas.h file.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
