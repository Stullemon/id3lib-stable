// $Id: header.h,v 1.5 1999/11/15 20:18:59 scott Exp $

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

#ifndef ID3LIB_HEADER_H
#define ID3LIB_HEADER_H

#include <id3/types.h>

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
// Revision 1.5  1999/11/15 20:18:59  scott
// Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
