// $Id: frame_parse.cpp,v 1.7 2000/05/08 04:07:39 eldamitri Exp $

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

#include <zlib.h>
#include "frame.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

void ID3_Frame::Parse(const uchar *buffer, luint size)
{
  luint i;
  luint posn = 0;
  
  for (i = 0; i < __num_fields; i++)
  {
    __fields[i]->SetSpec(__hdr.GetSpec());
    posn += __fields[i]->Parse(buffer, posn, size);
    
    // if we just parsed a TEXTENC field, we'd better tell the rest of the
    // concerned string fields in the frame what they are expected to parse
    // (ASCII or Unicode)
    if (ID3FN_TEXTENC == __fields[i]->__id)
    {
      UpdateStringTypes();
    }
  }
  
  __changed = false;
  
  return ;
}
