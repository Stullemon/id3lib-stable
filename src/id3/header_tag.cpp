// $Id: header_tag.cpp,v 1.10 1999/12/27 05:32:13 scott Exp $
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

#include <string.h>
#include <memory.h>
#include "header_tag.h"
#include "error.h"
#include "misc_support.h"

// Analyses a buffer to determine if we have a valid ID3v2 tag header.
// If so, return the number of bytes (starting _after_ the header) to
// read so we get all of the tag

lsint ID3_IsTagHeader(uchar header[ID3_TAGHEADERSIZE])
{
  lsint tagSize = -1;
  
  if ((memcmp(ID3_TAGID, header, ID3_TAGIDSIZE) == 0) &&
      (header[ID3_TAGIDSIZE] <= ID3v2_VERSION))
  {
    int28 temp = &header[6];
    tagSize = temp.get();
  }
    
  return tagSize;
}


size_t ID3_TagHeader::Size(void)
{
  size_t bytesUsed = ID3_TAGHEADERSIZE;
  
  if (__pInfo->bHasExtHeader)
  {
    bytesUsed += __pInfo->ulExtHeaderBytes + sizeof(uint32);
  }
    
  return bytesUsed;
}


size_t ID3_TagHeader::Render(uchar *buffer)
{
  size_t bytesUsed = 0;
  
  memcpy(&buffer[bytesUsed], (uchar *) ID3_TAGID, strlen(ID3_TAGID));
  bytesUsed += strlen(ID3_TAGID);
  
  buffer[bytesUsed++] = __ucVersion;
  buffer[bytesUsed++] = __ucRevision;
  
  // do the automatic flags
  if (__pInfo->bSetExpBit)
  {
    __ulFlags |= ID3HF_EXPERIMENTAL;
  }
    
  if (__pInfo->bHasExtHeader)
  {
    __ulFlags |= ID3HF_EXTENDEDHEADER;
  }
    
  // set the flags byte in the header
  buffer[bytesUsed++] = (uchar)(__ulFlags & MASK8);
  
  int28 temp = __ulDataSize;
  
  for (size_t i = 0; i < sizeof(uint32); i++)
  {
    buffer[bytesUsed++] = temp[i];
  }
    
  // now we render the extended header
  if (__pInfo->bHasExtHeader)
  {
    RenderNumber(&buffer[bytesUsed], __pInfo->ulExtHeaderBytes);
    bytesUsed += sizeof(uint32);
  }
  
  bytesUsed = Size();
  
  return bytesUsed;
}

// $Log: header_tag.cpp,v $
// Revision 1.10  1999/12/27 05:32:13  scott
// (Size, Render): Minor return type change.
// (ID3_IsTagHeader): Updated for new version constants.
//
// Revision 1.9  1999/12/26 15:11:33  scott
// (Render): Now uses RenderNumber, defined in misc_support.
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
// Revision 1.5  1999/11/15 20:19:36  scott
// Added include for config.h.  Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
