// $Id: writers_compressed.h,v 1.2 2000/10/12 22:29:33 eldamitri Exp $

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

#ifndef _ID3LIB_WRITERS_COMPRESSED_H_
#define _ID3LIB_WRITERS_COMPRESSED_H_

#include <zlib.h>
#include "writer_decorators.h"

namespace dami
{
  namespace io
  {
    class CompressedWriter : public IdentityWriter
    {
      typedef IdentityWriter SUPER;
      String _data;
      size_type _origSize;
     public:

      CompressedWriter(ID3_Writer& writer) : SUPER(writer), _data(), _origSize(0) { ; }
      virtual ~CompressedWriter() { this->flush(); }
      
      size_type getOrigSize() const
      {
        return _origSize;
      }

      void flush()
      {
        if (_data.size() == 0)
        {
          return;
        }
        const char_type* data = reinterpret_cast<const char_type*>(_data.data());
        size_type dataSize = _data.size();
        _origSize = dataSize;
        // The zlib documentation specifies that the destination size needs to
        // be an unsigned long at least 0.1% larger than the source buffer,
        // plus 12 bytes
        unsigned long newDataSize = dataSize + (dataSize / 10) + 12;
        char_type* newData = new char_type[newDataSize];
        if (::compress(newData, &newDataSize, data, dataSize) != Z_OK)
        {
          // log this
          ID3D_WARNING("io::CompressedWriter: error compressing");
          SUPER::writeChars(data, dataSize);
        }
        else if (newDataSize < dataSize)
        {
          ID3D_NOTICE("io::CompressedWriter: compressed size = " << newDataSize << ", original size = " << dataSize ); 
          SUPER::writeChars(newData, newDataSize);
        }
        else
        {
          ID3D_NOTICE("io::CompressedWriter: no compression!compressed size = " << newDataSize << ", original size = " << dataSize ); 
          SUPER::writeChars(data, dataSize);
        }
        delete [] newData;
        _data.erase();
      }
      
      int_type writeChar(char_type ch)
      {
        this->writeChars(&ch, 1);
        return ch;
      }

      size_type writeChars(const char_type buf[], size_type len)
      {
        return this->writeChars(reinterpret_cast<const char*>(buf), len);
      }
      virtual size_type writeChars(const char buf[], size_type len)
      { 
        ID3D_NOTICE("io::CompressedWriter: writing chars: " << len );
        _data.append(buf, len);
        return len;
      }

      pos_type getCur() { return _data.size(); }
    };
  };
};

#endif /* _ID3LIB_WRITERS_COMPRESSED_H_ */
