// $Id: frame_parse.cpp,v 1.3 2000/04/26 03:42:52 eldamitri Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#include <iostream.h>
#include <zlib.h>
#include "frame.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

void ID3_Frame::Parse(uchar *buffer, luint size)
{
  luint i;
  luint posn = 0;
  
  for (i = 0; i < __ulNumFields; i++)
  {
    __apFields[i]->SetSpec(__FrmHdr.GetSpec());
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
// Revision 1.3  2000/04/26 03:42:52  eldamitri
// - Replaced version/revision uchar combination with ID3_V2Spec enums
// - Deprecated {Get,Set}Version, GetRevision for {Get,Set}Spec
// - ID3_VerCtl enumeration deprecated in favor of using two ID3_V2Spec
//   enums to denote field scope
// - Replaced ID3v2_VERSION, ID3v2_REVISION constants with ID3V2_LATEST
//   enum
// - Use ID3V2_UNKNOWN enum rather than 0 for version, revision
//
// Revision 1.2  2000/04/18 22:11:30  eldamitri
// Moved frame_parse.cpp from src/id3/ to src/
//
// Revision 1.12  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.11  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
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
