// $Id: readers.h,v 1.1 2000/09/27 04:19:28 eldamitri Exp $

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

#ifndef _ID3LIB_READERS_H_
#define _ID3LIB_READERS_H_

#include <fstream.h>
#include "reader.h"
#include "utils.h"
#include <zlib.h>

namespace id3
{
  typedef unsigned char uchar;
  typedef unsigned long int luint;

  class IStreamReader : public ID3_Reader
  {
    istream& _stream;
   protected:
    istream& getReader() const { return _stream; }
   public:
    IStreamReader(istream& reader) : _stream(reader) { ; }
    virtual ~IStreamReader() { ; }
    virtual void close() { ; }
    
    virtual int_type peekChar() { return _stream.peek(); }
    
    /** Read up to \c len chars into buf and advance the internal position
     ** accordingly.  Returns the number of characters read into buf.
     **/
    virtual streamsize readChars(char_type buf[], streamsize len)
    {
      _stream.read(buf, len);
      return _stream.gcount();
    }
    
    virtual pos_type getCur() { return _stream.tellg(); }
    
    /** Set the value of the internal position for reading.
     **/
    virtual pos_type setCur(pos_type pos) { _stream.seekg(pos); return pos; }
  };
  
  class IFStreamReader : public IStreamReader
  {
   public:
    IFStreamReader(ifstream& reader) : IStreamReader(reader) { ; }
    
    virtual void close() 
    { 
      dynamic_cast<ifstream&>(this->getReader()).close(); 
    }
  };
  
  class MemoryReader : public ID3_Reader
  {
    const char* _beg;
    const char* _cur;
    const char* _end;
   protected:
    void setBuffer(const char* buf, size_t size)
    {
      _beg = buf;
      _cur = buf;
      _end = buf + size;
    };
    MemoryReader()
    {
      this->setBuffer(NULL, 0);
    }
   public:
    MemoryReader(const char* buf, size_t size)
    {
      this->setBuffer(buf, size);
    };
    virtual ~MemoryReader() { ; }
    virtual void close() { ; }
    
    virtual int_type peekChar() 
    { 
      if (_cur < _end)
      {
        return *_cur; 
      }
      return END_OF_READER;
    }
    
    /** Read up to \c len chars into buf and advance the internal position
     ** accordingly.  Returns the number of characters read into buf.
     **/
    virtual streamsize readChars(char_type buf[], streamsize len)
    {
      streamsize size = id3::min<streamsize>(len, _end - _cur);
      ::memcpy(buf, _cur, size);
      _cur += size;
      return size;
    }
    
    virtual pos_type getCur() 
    { 
      return _cur - _beg; 
    }
    
    virtual pos_type getBeg()
    {
      return 0;
    }
    
    virtual pos_type getEnd()
    {
      return _end - _beg;
    }
    
    /** Set the value of the internal position for reading.
     **/
    virtual pos_type setCur(pos_type pos) 
    {
      _cur = _beg + id3::min(pos, this->getEnd());
      return this->getCur();
    }
  };
  
  class CompressedMemoryReader : public MemoryReader
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

  class CompressedStreamReader : public MemoryReader
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

#endif /* _ID3LIB_READERS_H_ */
