// $Id: header.cpp,v 1.5 1999/11/15 20:18:46 scott Exp $

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
#include <id3/header.h>
#include <id3/error.h>

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
// Revision 1.5  1999/11/15 20:18:46  scott
// Added include for config.h.  Minor code cleanup.  Made private
// member variable names more descriptive.  Fixed bug I introduced in
// logic of checking version/revision numbers in
// ID3_LookupHeaderInfo, and cleaned up the code in the process.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
