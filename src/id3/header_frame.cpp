// $Id: header_frame.cpp,v 1.5 1999/11/15 20:19:08 scott Exp $

//  The authors have released ID3Lib as Public Domain (PD) and claim no
//  copyright, patent or other intellectual property protection in this work.
//  This means that it may be modified, redistributed and used in commercial
//  and non-commercial software and hardware without restrictions.  ID3Lib is
//  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
//  express or implied.
//
//  The ID3Lib authors encourage improvements and optimisations to be sent to
//  the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these
//  terms.
//
//  Mon Nov 23 18:34:01 1998

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <memory.h>
#include <id3/header_frame.h>
#include <id3/error.h>

void ID3_FrameHeader::SetFrameID(ID3_FrameID id)
{
  __eFrameID = id;
  
  return ;
}


luint ID3_FrameHeader::Size(void)
{
  return __pInfo->ucFrameIDBytes + __pInfo->ucFrameSizeBytes + __pInfo->ucFrameFlagsBytes;
}


luint ID3_FrameHeader::GetFrameInfo(ID3_FrameAttr &attr, uchar *buffer)
{
  luint posn = 0;
  luint i = 0;
  
  strncpy(attr.sTextID, (char *) buffer, __pInfo->ucFrameIDBytes);

  attr.sTextID[__pInfo->ucFrameIDBytes] = 0;
  
  posn += __pInfo->ucFrameIDBytes;
  
  attr.ulSize = 0;
  
  for(i = 0; i < __pInfo->ucFrameSizeBytes; i++)
    attr.ulSize |= buffer[posn + i] << ((__pInfo->ucFrameSizeBytes - 1 - i) * 8);
    
  posn += __pInfo->ucFrameSizeBytes;
  
  attr.ulFlags = 0;
  
  for(i = 0; i < __pInfo->ucFrameFlagsBytes; i++)
    attr.ulFlags |= buffer[posn + i] << ((__pInfo->ucFrameFlagsBytes - 1 - i) * 8);
    
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
    ID3_THROW(ID3E_InvalidFrameID);

  if(__pInfo->ucFrameIDBytes < strlen(frameDef->sLongTextID))
    textID = frameDef->sShortTextID;
  else
    textID = frameDef->sLongTextID;
    
  memcpy(&buffer[bytesUsed], (uchar *) textID, __pInfo->ucFrameIDBytes);
  bytesUsed += __pInfo->ucFrameIDBytes;
  
  for(i = 0; i < __pInfo->ucFrameSizeBytes; i++)
    buffer[bytesUsed + i] = 
      (uchar)((__ulDataSize >> ((__pInfo->ucFrameSizeBytes - i - 1) * 8)) & 0xFF);
    
  bytesUsed += __pInfo->ucFrameSizeBytes;
  
  for(i = 0; i < __pInfo->ucFrameFlagsBytes; i++)
    buffer[bytesUsed + i] = 
      (uchar)((__ulFlags >> ((__pInfo->ucFrameFlagsBytes - i - 1) * 8)) & 0xFF);
    
  bytesUsed += __pInfo->ucFrameFlagsBytes;
  
  return bytesUsed;
}



// $Log: header_frame.cpp,v $
// Revision 1.5  1999/11/15 20:19:08  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
