// $Id: frame_parse.cpp,v 1.10 1999/12/27 05:55:57 scott Exp $
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

#include "frame.h"
#include <zlib.h>

void ID3_Frame::Parse(uchar *buffer, luint size)
{
  luint i;
  luint posn = 0;
  
  for (i = 0; i < __ulNumFields; i++)
  {
    __apFields[i]->SetVersion(__FrmHdr.GetVersion(), __FrmHdr.GetRevision());
    posn += __apFields[i]->Parse(buffer, posn, size);
    
    // if we just parsed a TEXTENC field, we'd better tell the rest of the
    // concerned string fields in the frame what they are expected to parse
    // (ASCII or Unicode)
    if (ID3FN_TEXTENC == __apFields[i]->__eName)
    {
      UpdateStringTypes();
    }
  }
  
  __bHasChanged = false;
  
  return ;
}

// $Log: frame_parse.cpp,v $
// Revision 1.10  1999/12/27 05:55:57  scott
// (Parse): Moved header processing tasks to the header object, now a
// member of ID3_Frame.
//
// Revision 1.9  1999/12/26 15:10:48  scott
// Minor reformatting.
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
// Revision 1.5  1999/11/15 20:18:19  scott
// Added include for config.h.  Made variable names more descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
