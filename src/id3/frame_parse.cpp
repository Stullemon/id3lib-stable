// $Id: frame_parse.cpp,v 1.5 1999/11/15 20:18:19 scott Exp $

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

#include <id3/frame.h>
#include <zlib.h>

void ID3_Frame::Parse(uchar *buffer, luint size)
{
  luint i;
  luint posn = 0;
  
  for (i = 0; i < __ulNumFields; i++)
  {
    __apFields[i]->SetVersion(__ucVersion, __ucRevision);
    posn += __apFields[i]->Parse(buffer, posn, size);
    
    // if we just parsed a TEXTENC field, we'd better tell the rest of the
    // concerned string fields in the frame what they are expected to parse
    // (ASCII or Unicode)
    if (ID3FN_TEXTENC == __apFields[i]->__eName)
      UpdateStringTypes();
  }
  
  __bHasChanged = false;
  
  return ;
}

// $Log: frame_parse.cpp,v $
// Revision 1.5  1999/11/15 20:18:19  scott
// Added include for config.h.  Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
