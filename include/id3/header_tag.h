// $Id: header_tag.h,v 1.8 2000/04/21 05:46:16 eldamitri Exp $

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

#ifndef __ID3LIB_HEADER_TAG_H__
#define __ID3LIB_HEADER_TAG_H__

#include "header.h"

#define ID3HF_UNSYNC            (1 << 7)
#define ID3HF_EXTENDEDHEADER    (1 << 6)
#define ID3HF_EXPERIMENTAL      (1 << 5)

class ID3_TagHeader : public ID3_Header
{
public:
  virtual size_t Size(void);
  virtual size_t Render(uchar *buffer);
  ID3_TagHeader& operator=(const ID3_TagHeader&);
};

lsint ID3_IsTagHeader(uchar header[ID3_TAGHEADERSIZE]);

#endif /* __ID3LIB_HEADER_TAG_H__ */
