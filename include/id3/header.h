// $Id: header.h,v 1.3 1999/12/17 16:05:02 scott Exp $
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

#ifndef ID3LIB_HEADER_H
#define ID3LIB_HEADER_H

#include "types.h"

#define ID3_TAGVERSION  (3)
#define ID3_TAGREVISION (0)

struct ID3_HeaderInfo
{
  uchar ucVersion;
  uchar ucRevision;
  uchar ucFrameIDBytes;
  uchar ucFrameSizeBytes;
  uchar ucFrameFlagsBytes;
  bool  bHasExtHeader;
  luint ulExtHeaderBytes;
  bool  bSetExpBit;
};

extern ID3_HeaderInfo ID3_VersionInfo[];

class ID3_Header
{
public:
  ID3_Header(void);
  
  void SetVersion(uchar ver, uchar rev);
  void SetDataSize(luint newSize);
  void SetFlags(luint newFlags);
  virtual luint Size(void) = 0;
  virtual luint Render(uchar *buffer) = 0;
  
protected:
  uchar __ucVersion;        // which version?
  uchar __ucRevision;       // which revision?
  luint __ulDataSize;       // how big is the data?
  luint __ulFlags;          // tag flags
  ID3_HeaderInfo *__pInfo;  // the info about this version of the headers
}
;

ID3_HeaderInfo *ID3_LookupHeaderInfo(uchar ver, uchar rev);

#endif

// $Log: header.h,v $
// Revision 1.3  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:04  scott
// moved from src/id3 to include/id3
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:18:59  scott
// Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
