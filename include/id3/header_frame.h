// $Id: header_frame.h,v 1.1 1999/12/01 17:16:05 scott Exp $
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

#ifndef ID3LIB_HEADER_FRAME_H
#define ID3LIB_HEADER_FRAME_H

#include <id3/types.h>
#include <id3/header.h>
#include <id3/header_tag.h>
#include <id3/field.h>

#define ID3FL_TAGALTER    (1 << 15)
#define ID3FL_FILEALTER   (1 << 14)
#define ID3FL_SIGNED      (1 << 13)
#define ID3FL_COMPRESSION (1 <<  7)
#define ID3FL_ENCRYPTION  (1 <<  6)
#define ID3FL_GROUPING    (1 <<  5)

struct ID3_FrameAttr
{
  char  sTextID[5];
  luint ulSize;
  luint ulFlags;
};

class ID3_FrameHeader : public ID3_Header
{
public:
  virtual luint Size(void);
  void SetFrameID(ID3_FrameID id);
  luint GetFrameInfo(ID3_FrameAttr &attr, uchar *buffer);
  virtual luint Render(uchar *buffer);
  
protected:
  ID3_FrameID   __eFrameID;        // which frame are we the header for?
}
;

#endif

// $Log: header_frame.h,v $
// Revision 1.1  1999/12/01 17:16:05  scott
// moved from src/id3 to include/id3
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:19:25  scott
// Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
