// $Id: header_tag.cpp,v 1.6 1999/11/29 19:26:18 scott Exp $
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

#include <string.h>
#include <memory.h>
#include <id3/header_tag.h>
#include <id3/error.h>

// Analyses a buffer to determine if we have a valid ID3v2 tag header.
// If so, return the number of bytes (starting _after_ the header) to
// read so we get all of the tag

lsint ID3_IsTagHeader(uchar header[ID3_TAGHEADERSIZE])
{
  lsint tagSize = -1;
  
  if (memcmp("ID3", header, 3) == 0)
    if (header[3] <= ID3_TAGVERSION)
    {
      int28 temp = &header[6];
      
      tagSize = temp.get();
    }
    
  return tagSize;
}


luint ID3_TagHeader::Size(void)
{
  luint bytesUsed = ID3_TAGHEADERSIZE;
  
  if (__pInfo->bHasExtHeader)
    bytesUsed += __pInfo->ulExtHeaderBytes + sizeof(luint);
    
  return bytesUsed;
}


luint ID3_TagHeader::Render(uchar *buffer)
{
  luint bytesUsed = 0;
  
  memcpy(&buffer[bytesUsed], (uchar *) ID3_TAGID, strlen(ID3_TAGID));
  bytesUsed += strlen(ID3_TAGID);
  
  buffer[bytesUsed++] = __ucVersion;
  buffer[bytesUsed++] = __ucRevision;
  
  // do the automatic flags
  if (__pInfo->bSetExpBit)
    __ulFlags |= ID3HF_EXPERIMENTAL;
    
  if (__pInfo->bHasExtHeader)
    __ulFlags |= ID3HF_EXTENDEDHEADER;
    
  // set the flags byte in the header
  buffer[bytesUsed++] = (uchar)(__ulFlags & 0xFF);
  
  int28 temp = __ulDataSize;
  
  for (luint i = 0; i < sizeof(luint); i++)
    buffer[bytesUsed++] = temp[i];
    
  // now we render the extended header
  if (__pInfo->bHasExtHeader)
  {
    luint i;
    
    for (i = 0; i < sizeof(luint); i++)
      buffer[bytesUsed + i] = (uchar)((__pInfo->ulExtHeaderBytes >> 
                                       ((sizeof(luint) - i - 1) * 8)) & 0xFF);
      
    bytesUsed += i;
  }
  
  bytesUsed = Size();
  
  return bytesUsed;
}

// $Log: header_tag.cpp,v $
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:19:36  scott
// Added include for config.h.  Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
