// $Id: sized_types.h,v 1.2 1999/12/17 16:05:02 scott Exp $
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

/* 
 * This file defines size-specific typedefs based on the macros defined in
 * limits.h
 */

#include <limits.h>

/* define our datatypes */

/* Define 8-bit types */
#if UCHAR_MAX == 0xff

typedef unsigned char   uint8;
typedef signed char      int8;

#else
#error This machine has no 8-bit type
#endif /* UCHAR_MAX == 0xff */

/* Define 16-bit types */
#if UINT_MAX == 0xffff

typedef unsigned int    uint16;
typedef int              int16;

#elif USHRT_MAX == 0xffff

typedef unsigned short  uint16;
typedef short            int16;

#else
#error This machine has no 16-bit type
#endif /* UINT_MAX == 0xffff */

/* Define 32-bit types */
#if UINT_MAX == 0xfffffffful

typedef unsigned int    uint32;
typedef int              int32;

#elif ULONG_MAX == 0xfffffffful

typedef unsigned long   uint32;
typedef long             int32;

#elif USHRT_MAX == 0xfffffffful

typedef unsigned short  uint32;
typedef short            int32;

#else
#error This machine has no 32-bit type
#endif /* UINT_MAX == 0xfffffffful */
