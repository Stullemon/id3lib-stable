// $Id: header_frame.cpp,v 1.9 1999/12/26 15:11:26 scott Exp $
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
#include "header_frame.h"
#include "error.h"
#include "misc_support.h"

void ID3_FrameHeader::SetFrameID(ID3_FrameID id)
{
  __eFrameID = id;
  
  return ;
}


luint ID3_FrameHeader::Size(void)
{
  return __pInfo->ucFrameIDBytes + __pInfo->ucFrameSizeBytes + 
    __pInfo->ucFrameFlagsBytes;
}


luint ID3_FrameHeader::GetFrameInfo(ID3_FrameAttr &attr, uchar *buffer)
{
  luint posn = 0;
  luint i = 0;
  
  strncpy(attr.sTextID, (char *) buffer, __pInfo->ucFrameIDBytes);

  attr.sTextID[__pInfo->ucFrameIDBytes] = 0;
  
  posn += __pInfo->ucFrameIDBytes;
  
  attr.ulSize = 0;
  
  attr.ulSize = ParseNumber(&buffer[posn], __pInfo->ucFrameSizeBytes);
    
  posn += __pInfo->ucFrameSizeBytes;
  
  attr.ulFlags = ParseNumber(&buffer[posn], __pInfo->ucFrameFlagsBytes);
    
  posn += __pInfo->ucFrameFlagsBytes;
  
  return posn;
}


luint ID3_FrameHeader::Render(uchar *buffer)
{
  luint bytesUsed = 0;
  ID3_FrameDef *frameDef = NULL;
  char *textID = NULL;
  luint i;

  frameDef = ID3_FindFrameDef(__eFrameID);
  if (NULL == frameDef)
  {
    ID3_THROW(ID3E_InvalidFrameID);
  }

  if(__pInfo->ucFrameIDBytes < strlen(frameDef->sLongTextID))
  {
    textID = frameDef->sShortTextID;
  }
  else
  {
    textID = frameDef->sLongTextID;
  }
    
  memcpy(&buffer[bytesUsed], (uchar *) textID, __pInfo->ucFrameIDBytes);
  bytesUsed += __pInfo->ucFrameIDBytes;
  
  RenderNumber(&buffer[bytesUsed], __ulDataSize, __pInfo->ucFrameSizeBytes);
  bytesUsed += __pInfo->ucFrameSizeBytes;
  
  RenderNumber(&buffer[bytesUsed], __ulFlags, __pInfo->ucFrameFlagsBytes);
  bytesUsed += __pInfo->ucFrameFlagsBytes;
  
  return bytesUsed;
}



// $Log: header_frame.cpp,v $
// Revision 1.9  1999/12/26 15:11:26  scott
// (GetFrameInfo): Now uses ParseNumber, defined in misc_support.
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
// Revision 1.5  1999/11/15 20:19:08  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
