// $Id: writer_decorators.h,v 1.2 2000/10/12 22:29:33 eldamitri Exp $

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

#ifndef _ID3LIB_WRITER_DECORATORS_H_
#define _ID3LIB_WRITER_DECORATORS_H_

#include "debug.h"
#include "strings.h"
#include "writer.h"
#include "utils.h"
#include "globals.h"

namespace dami
{
  namespace io
  {
    class IdentityWriter : public ID3_Writer
    {
      ID3_Writer& _writer;
     public:
      IdentityWriter(ID3_Writer& writer) : _writer(writer) { }
      virtual ~IdentityWriter() { ; }
      
      virtual void close() { _writer.close(); }
      virtual void flush() { _writer.flush(); }
      virtual pos_type getBeg() { return _writer.getBeg(); }
      virtual pos_type getEnd() { return _writer.getEnd(); }
      virtual pos_type getCur() { return _writer.getCur(); }
      
      virtual int_type writeChar(char_type ch) { return _writer.writeChar(ch); }
      virtual size_type writeChars(const char_type buf[], size_type len)
      { return _writer.writeChars(buf, len); }
      virtual size_type writeChars(const char buf[], size_type len)
      { 
        return this->writeChars(reinterpret_cast<const char_type *>(buf), len);
      }
    };

    class CharWriter : public IdentityWriter
    {
      typedef IdentityWriter SUPER;
     public:

      CharWriter(ID3_Writer& writer) : SUPER(writer) { }
      virtual ~CharWriter() { ; }
    
      /**
       * Write \c len characters into the array \c buf.  Since the stream needs
       * might have been unsynced, this function copies the characters one at a
       * time.
       */
      virtual size_type writeChars(const char buf[], size_type len)
      { 
        return this->writeChars(reinterpret_cast<const char_type *>(buf), len);
      }
      size_type writeChars(const char_type buf[], size_type len)
      {
        size_t numChars = 0;
        ID3D_NOTICE( "CharWriter::writeChars(): len = " << len );
        for (; numChars < len; ++numChars)
        {
          if (this->atEnd())
          {
            break;
          }
          this->writeChar(buf[numChars]);
        }
        ID3D_NOTICE( "CharWriter::writeChars(): numChars = " << len );
        return numChars;
      }
    };

    class UnsyncedWriter : public CharWriter
    {
      typedef CharWriter SUPER;
      int_type _last;
      size_type _numSyncs;
     public:

      UnsyncedWriter(ID3_Writer& writer) : SUPER(writer), _last('\0'), _numSyncs(0) { }

      size_type getNumSyncs() const
      {
        return _numSyncs;
      }

      int_type writeChar(char_type ch)
      {
        if (_last == 0xFF && (ch == 0x00 || ch >= 0xE0))
        {
          SUPER::writeChar('\0');
          _numSyncs++;
        }
        _last = SUPER::writeChar(ch);
        return _last;
      }

      void flush()
      {
        if (_last == 0xFF)
        {
          _last = SUPER::writeChar('\0');
          _numSyncs++;
        }
        SUPER::flush();
      }

      /**
       * Write \c len characters into the array \c buf.  Since the stream needs
       * might have been unsynced, this function copies the characters one at a
       * time.
       */
      virtual size_type writeChars(const char buf[], size_type len)
      { 
        return this->writeChars(reinterpret_cast<const char_type *>(buf), len);
      }
      size_type writeChars(const char_type buf[], size_type len)
      {
        pos_type beg = this->getCur();
        ID3D_NOTICE( "UnsyncedWriter::writeChars(): len = " << len );
        for (size_t i = 0; i < len; ++i)
        {
          if (this->atEnd())
          {
            break;
          }
          this->writeChar(buf[i]);
        }
        size_type numChars = this->getCur() - beg;
        ID3D_NOTICE( "CharWriter::writeChars(): numChars = " << numChars );
        return numChars;
      }
    };

    class BinaryNumberWriter : public IdentityWriter
    {
      typedef IdentityWriter SUPER;
     public:
      BinaryNumberWriter(ID3_Writer& writer) : SUPER(writer) { ; }
      virtual ~BinaryNumberWriter() { ; }

      size_type writeNumber(uint32 val, size_t numBytes)
      {
        char_type bytes[sizeof(uint32)];
        size_type size = min<size_type>(numBytes, sizeof(uint32));
        renderNumber(bytes, val, size);
        return this->writeChars(bytes, size);
      }
    };

    class TrailingSpacesWriter : public IdentityWriter
    {
      typedef IdentityWriter SUPER;
     public:
      TrailingSpacesWriter(ID3_Writer& writer) : SUPER(writer) { ; }
      virtual ~TrailingSpacesWriter() { ; }

      virtual size_type writeChars(const char buf[], size_type len)
      { 
        return this->writeChars(reinterpret_cast<const char_type *>(buf), len);
      }
      size_type writeChars(const char_type buf[], size_type len)
      {
        String str((const char*)buf);
        return this->writeString(str, len);
      }
      
      size_type writeString(String buf, size_type len)
      {
        size_type strLen = buf.size();
        size_type size = min(len, strLen);
        SUPER::writeChars((const uchar*)buf.data(), size);
        for (; size < len; ++size)
        {
          SUPER::writeChar(' ');
        }
      }
    };

    class StringWriter : public ID3_Writer
    {
      String& _string;
     public:
      StringWriter(String& string) : _string(string) { ; }
      virtual ~StringWriter() { ; }

      void close() { ; }
      void flush() { ; }
      virtual size_type writeChars(const char buf[], size_type len)
      { 
        return this->writeChars(reinterpret_cast<const char_type *>(buf), len);
      }
      size_type writeChars(const char_type buf[], size_type len)
      {
        ID3D_NOTICE("io::StringWriter: writing chars: " << len );
        _string.append(reinterpret_cast<const char*>(buf), len);
        return len;
      }

      pos_type getCur()
      {
        return _string.size();
      }
    };
  };
};

#endif /* _ID3LIB_WRITER_DECORATORS_H_ */
