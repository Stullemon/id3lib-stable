// $Id: reader_decorators.h,v 1.1 2000/09/22 04:40:04 eldamitri Exp $

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

#ifndef _ID3LIB_READER_DECORATORS_H_
#define _ID3LIB_READER_DECORATORS_H_

#include "reader.h"

namespace
{
  template<typename T>
  const T& min(const T& a, const T& b)
  {
    return (a < b) ? a : b;
  }
}

namespace id3
{
  class IdentityReader : public ID3_Reader
  {
    ID3_Reader& _reader;
   public:
    IdentityReader(ID3_Reader& reader) : _reader(reader) { }
    virtual ~IdentityReader() { _reader.close(); }
  
    virtual void close() { _reader.close(); }
    virtual int_type readByte() { return _reader.readByte(); }
    virtual int_type peek() { return _reader.peek(); }
    virtual streamsize readBytes(char_type buf[], streamsize len)
    { return _reader.readBytes(buf, len); }
  
    virtual int_type skipBytes(streamsize len) { return _reader.skipBytes(len); }
    virtual pos_type tell() { return _reader.tell(); }
    virtual pos_type seek(pos_type pos) { return _reader.seek(pos); }
  };

  /**
   * Set a window on the buffer.  Characters can only be read within this 
   * window.
   */
  class WindowedReader : public IdentityReader
  {
    pos_type _beg, _end;
    typedef IdentityReader SUPER;
   private:
    bool inWindow(pos_type cur) 
    { 
      return _beg <= cur < _end;
    }
   public:
    WindowedReader(ID3_Reader& reader) 
    : SUPER(reader),
    _beg(0), 
    _end(static_cast<pos_type>(-1))
    {
    }
  
    void setWindow(pos_type beg, pos_type end)
    {
      if (beg <= end)
      {
        _beg = beg;
        _end = end;
      }
    }
    pos_type setBeg(pos_type beg)
    {
      if (beg <= _end)
      {
        _beg = beg;
      }
      return _beg;
    }
    pos_type setEnd(pos_type end)
    {
      if (_beg <= end)
      {
        _end = end;
      }
    }
    pos_type getBeg() const
    {
      return _beg;
    }
  
    pos_type getEnd() const
    {
      return _end;
    }

    pos_type seekBeg() 
    {
      return SUPER::seek(this->getBeg());
    }

    pos_type seekEnd()
    {
      return SUPER::seek(this->getEnd());
    }

    bool inWindow() 
    { 
      return this->inWindow(this->tell());
    }

    int_type readChar()
    {
      int_type ch = EOF;
      if (this->inWindow())
      {
        ch = SUPER::readChar();
      }
      return ch;
    }

    int_type peek()
    {
      int_type ch = EOF;
      if (this->inWindow())
      {
        ch = SUPER::peek();
      }
      return ch;
    }

    streamsize readChars(char_type buf[], streamsize len)
    {
      pos_type cur = this->tell();
      streamsize size = 0;
      if (this->inWindow(cur))
      {
        size = SUPER::readChars(buf, min<streamsize>(len, _end - cur));
      }
      return size;
    }
  };

  class UnsyncedReader : public IdentityReader
  {
   public:
    typedef IdentityReader SUPER;

    UnsyncedReader(ID3_Reader& reader) : SUPER(reader) { }
    int_type readChar()
    {
      int_type ch = this->peek();
      if (ch == EOF)
      {
        return ch;
      }
      ch = SUPER::readChar();
      if (ch == 0xFF && this->peek() == 0x00)
      {
        SUPER::readChar();
      }
      return ch;
    }
  
    streamsize readChars(char_type buf[], streamsize len)
    {
      size_t numChars = 0;
      for (; numChars < len; ++numChars)
      {
        if (this->peek() == EOF)
        {
          break;
        }
        buf[numChars] = this->readChar();
      }
      return numChars;
    }
  };

};

#endif /* _ID3LIB_READER_DECORATORS_H_ */
