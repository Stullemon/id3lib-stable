// $Id: readers_compressed.h,v 1.1 2000/10/02 17:08:13 eldamitri Exp $

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

#include "readers.h"

namespace dami
{
  namespace io
  {
    class CompressedMemoryReader : public ID3_MemoryReader
    {
      char* _uncompressed;
     public:
      
      CompressedMemoryReader(const char* buffer, size_t size, size_t new_size)
      : _uncompressed(new char[new_size])
      {
        ::uncompress(reinterpret_cast<uchar*>(_uncompressed),
                     reinterpret_cast<luint*>(&new_size),
                     reinterpret_cast<const uchar*>(buffer),
                     size);
        this->setBuffer(_uncompressed, new_size);
      }
      
      CompressedMemoryReader(const uchar* buffer, size_t size, size_t new_size)
      : _uncompressed(new char[new_size])
      {
        ::uncompress(reinterpret_cast<uchar*>(_uncompressed),
                     reinterpret_cast<luint*>(&new_size), buffer,
                     size);
        this->setBuffer(_uncompressed, new_size);
      }
      
      virtual ~CompressedMemoryReader() 
      { 
        delete [] _uncompressed; 
      }
      
    };
    
    class CompressedStreamReader : public ID3_MemoryReader
    {
      char* _uncompressed;
     public:
      
      CompressedStreamReader(ID3_Reader& reader, streamsize new_size)
      : _uncompressed(new char[new_size])
      {
        size_t old_size = reader.remainingChars();
        uchar* buffer = new uchar[old_size];
        reader.readChars(buffer, old_size);
        ::uncompress(reinterpret_cast<uchar*>(_uncompressed),
                     reinterpret_cast<luint*>(&new_size),
                     reinterpret_cast<const uchar*>(buffer),
                     old_size);
        this->setBuffer(_uncompressed, new_size);
        delete [] buffer;
      }
      
      virtual ~CompressedStreamReader() 
      { 
        delete [] _uncompressed; 
      }
      
    };
  };
};

#endif /* _ID3LIB_READERS_COMPRESSED_H_ */
