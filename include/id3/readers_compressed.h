// $Id: readers_compressed.h,v 1.3 2000/10/12 22:29:33 eldamitri Exp $

// id3lib: a software library for creating and manipulating id3v1/v2 tags
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

#ifndef _ID3LIB_READERS_COMPRESSED_H_
#define _ID3LIB_READERS_COMPRESSED_H_

#include <zlib.h>
#include "readers.h"
#include "reader_decorators.h"

namespace dami
{
  namespace io
  {
    class CompressedReader : public ID3_MemoryReader
    {
      char_type* _uncompressed;
     public:
      
      CompressedReader(ID3_Reader& reader, size_type newSize)
        : _uncompressed(new char_type[newSize])
      {
        size_type oldSize = reader.remainingChars();
        
        BinaryReader br(reader);
        BString binary = br.readBinary(oldSize);

        ::uncompress(_uncompressed,
                     reinterpret_cast<luint*>(&newSize),
                     reinterpret_cast<const uchar*>(binary.data()),
                     oldSize);
        this->setBuffer(_uncompressed, newSize);
      }
      
      virtual ~CompressedReader() 
      { 
        delete [] _uncompressed; 
      }
    };
  };
};

#endif /* _ID3LIB_READERS_COMPRESSED_H_ */
