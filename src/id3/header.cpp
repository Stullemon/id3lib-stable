// $Id: header.cpp,v 1.8 1999/12/17 16:13:04 scott Exp $
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
#include "header.h"
#include "error.h"

ID3_HeaderInfo ID3_VersionInfo[] =
  {
    //         SIZEOF SIZEOF SIZEOF EXT    EXT  EXPERIM
    // VER REV FRID   FRSZ   FRFL   HEADER SIZE BIT
    {  2,  0,  3,     3,     0,     false, 0,   false },
    {  2,  1,  3,     3,     0,     true,  8,   true },
    {  3,  0,  4,     4,     2,     false, 10,  false },
    {  0 }
  };
  
  
ID3_HeaderInfo *ID3_LookupHeaderInfo(uchar ver, uchar rev)
{
  ID3_HeaderInfo *info = NULL;
  for (luint i = 0; ID3_VersionInfo[i].ucVersion != 0; i++)
    if (ID3_VersionInfo[i].ucVersion  == ver &&
        ID3_VersionInfo[i].ucRevision == rev)
    {
      info = &ID3_VersionInfo[i];
      break;
    }
    
  return info;
}

ID3_Header::ID3_Header(void)
{
  SetVersion(ID3_TAGVERSION, ID3_TAGREVISION);
  __ulDataSize = 0;
  __ulFlags = 0;
}

void ID3_Header::SetVersion(uchar ver, uchar rev)
{
  __ucVersion = ver;
  __ucRevision = rev;
  __pInfo = ID3_LookupHeaderInfo(__ucVersion, __ucRevision);
  
  return ;
}

void ID3_Header::SetDataSize(luint newSize)
{
  __ulDataSize = newSize;
  
  return ;
}

void ID3_Header::SetFlags(luint newFlags)
{
  __ulFlags = newFlags;
  
  return ;
}

// $Log: header.cpp,v $
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
// Revision 1.5  1999/11/15 20:18:46  scott
// Added include for config.h.  Minor code cleanup.  Made private
// member variable names more descriptive.  Fixed bug I introduced in
// logic of checking version/revision numbers in
// ID3_LookupHeaderInfo, and cleaned up the code in the process.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
