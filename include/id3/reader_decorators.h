// $Id: reader_decorators.h,v 1.2 2000/09/27 08:10:19 eldamitri Exp $

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

#include "debug.h"
#include "strings.h"
#include "reader.h"
#include "utils.h"
#include "globals.h"

namespace id3
{
  class IdentityReader : public ID3_Reader
  {
    ID3_Reader& _reader;
   protected:
    ID3_Reader& getReader() { return _reader; }
   public:
    IdentityReader(ID3_Reader& reader) : _reader(reader) { }
    virtual ~IdentityReader() { this->getReader().close(); }
  
    virtual void close() { this->getReader().close(); }
    virtual pos_type getBeg() { return this->getReader().getBeg(); }
    virtual pos_type getEnd() { return this->getReader().getEnd(); }
    virtual pos_type getCur() { return this->getReader().getCur(); }
    virtual pos_type setCur(pos_type pos) { return this->getReader().setCur(pos); }

    virtual int_type readChar() { return this->getReader().readChar(); }
    virtual int_type peekChar() { return this->getReader().peekChar(); }
    virtual streamsize readChars(char_type buf[], streamsize len)
    { return this->getReader().readChars(buf, len); }

    virtual streamsize skipChars(streamsize len) { return this->getReader().skipChars(len); }

    virtual streamsize remainingChars() { return this->getReader().remainingChars(); }
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
      return this->getBeg() <= cur && cur < this->getEnd();
    }
   public:
    WindowedReader(ID3_Reader& reader) 
    : SUPER(reader),
      _beg(reader.getBeg()),
      _end(reader.getEnd())
    {
    }
  
    void setWindow(pos_type beg, streamsize size)
    {
      ID3D_NOTICE( "id3::WindowedReader::setWindow() [beg, size] = [" << this->getBeg() << ", " << size << "]" );
      pos_type cur = this->getCur();

      // reset the end marker so as to avoid errors
      this->setEnd(SUPER::getEnd());

      // set the beginning marker
      this->setBeg(beg);

      // since the characters might be more than a byte in size, we need to 
      // manually get all the chars to set the window appropriately
      this->setCur(beg);
      ID3D_NOTICE( "id3::WindowedReader::setWindow(): after setCur(beg), cur = " << this->getCur() );
      
      this->skipChars(size);
      ID3D_NOTICE( "id3::WindowedReader::setWindow(): after skipChars, cur = " << this->getCur() );

      this->setEnd(this->getCur());

      ID3D_NOTICE( "id3::WindowedReader::setWindow() [beg, cur, end] = [" << this->getBeg() << ", " << this->getCur() << ", " << this->getEnd() << "]" );


      // reset the stream
      this->setCur(cur);
    }

    pos_type setBeg(pos_type beg)
    {
      // make sure the position we want to set to isn't past the current
      // end position or the superclass's beginning position
      if (beg <= _end && beg >= this->getReader().getBeg())
      {
        _beg = beg;
      }
      else
      {
        ID3D_WARNING( "id3::WindowedReader::setBeg() failed" );
      }
      return _beg;
    }
    pos_type setEnd(pos_type end)
    {
      // make sure the position we want to set to isn't beforen the current
      // beginning position or the superclass's end position
      if (this->getBeg() <= end && end <= this->getReader().getEnd())
      {
        _end = end;
      }
      else
      {
        ID3D_WARNING( "id3::WindowedReader::setEnd() failed, end = " << end );
        ID3D_WARNING( "id3::WindowedReader::setEnd() failed, beg = " << this->getBeg() );
        ID3D_WARNING( "id3::WindowedReader::setEnd() failed, super.end = " << SUPER::getEnd() );
        
      }
      return _end;
    }
    pos_type getBeg()
    {
      return _beg;
    }
  
    pos_type getEnd()
    {
      return _end;
    }

    bool inWindow() 
    { 
      return this->inWindow(this->getCur());
    }

    int_type readChar()
    {
      int_type ch = END_OF_READER;
      if (this->inWindow())
      {
        ch = this->getReader().readChar();
      }
      return ch;
    }

    int_type peekChar()
    {
      int_type ch = END_OF_READER;
      if (this->inWindow())
      {
        ch = this->getReader().peekChar();
      }
      return ch;
    }

    streamsize readChars(char_type buf[], streamsize len)
    {
      pos_type cur = this->getCur();
      streamsize size = 0;
      if (this->inWindow(cur))
      {
        size = this->getReader().readChars(buf, id3::min<streamsize>(len, _end - cur));
      }
      return size;
    }

    streamsize remainingChars()
    {
      pos_type cur = this->getCur();
      ID3D_NOTICE( "id3::WindowedReader::remainingChars(): cur = " << cur );
      streamsize size = this->skipChars(this->getEnd() - cur);
      ID3D_NOTICE( "id3::WindowedReader::remainingChars(): size = " << size );
      this->setCur(cur);
      return size;
    };
  };

  class UnsyncedReader : public IdentityReader
  {
    typedef IdentityReader SUPER;
   public:

    UnsyncedReader(ID3_Reader& reader) : SUPER(reader) { }
    int_type readChar()
    {
      int_type ch = this->peekChar();
      if (ch == END_OF_READER)
      {
        return ch;
      }
      ch = this->getReader().readChar();
      if (ch == 0xFF && this->peekChar() == 0x00)
      {
        ID3D_NOTICE( "id3::UnsyncedReader::readChar(): found sync at pos " << this->getCur() );
        this->getReader().readChar();
      }
      return ch;
    }
    
    streamsize skipChars(streamsize len)
    {
      ID3D_NOTICE( "id3::UnsyncedReader::skipChars(): len = " << len );
      return this->readChars(NULL, len);
    }

    /**
     * Read \c len characters into the array \c buf.  Since the stream needs
     * might have been unsynced, this function copies the characters one at a
     * time.
     */
    streamsize readChars(char_type buf[], streamsize len)
    {
      size_t numChars = 0;
      ID3D_NOTICE( "id3::UnsyncedReader::readChars(): len = " << len );
      for (; numChars < len; ++numChars)
      {
        if (this->peekChar() == END_OF_READER)
        {
          break;
        }
        char_type ch = this->readChar();
        if (buf != NULL)
        {
          buf[numChars] = ch;
        }
      }
      ID3D_NOTICE( "id3::UnsyncedReader::readChars(): numChars = " << len );
      return numChars;
    }

    streamsize remainingChars()
    {
      pos_type cur = this->getCur();
      ID3D_NOTICE( "id3::UnsyncedReader::remainingChars(): cur = " << cur );
      streamsize size = this->skipChars(this->getEnd() - cur);
      ID3D_NOTICE( "id3::UnsyncedReader::remainingChars(): size = " << size );
      this->setCur(cur);
      return size;
    };
  };

  class NumberReader : public IdentityReader
  {
    typedef IdentityReader SUPER;
   public:
    NumberReader(ID3_Reader& reader) : SUPER(reader) { ; }
    virtual ~NumberReader() { ; }

    uint32 readNumber(const size_t numChars)
    {
      uchar bytes[numChars];
      this->readChars(bytes, numChars);
      
      return id3::parseNumber(bytes, numChars);
    }
  };

  class TextReader : public IdentityReader
  {
    typedef IdentityReader SUPER;
    ID3_TextEnc _enc;
   public:
    TextReader(ID3_Reader& reader) : SUPER(reader), _enc(ID3TE_ASCII) { ; }
    virtual ~TextReader() { ; }
    void setEncoding(ID3_TextEnc enc) { _enc = enc; }
    ID3_TextEnc getEncoding() const { return _enc; }

    id3::string readText()
    {
      id3::string str;
      do
      {
        if (this->getEncoding() == ID3TE_ASCII)
        {
          int_type ch = this->readChar();
          if (ch == END_OF_READER || ch == '\0')
          {
            break;
          }
          str += static_cast<char>(ch);
        }
        else if (this->getEncoding() == ID3TE_UNICODE)
        {
          int_type ch1 = this->readChar();
          int_type ch2 = this->readChar();
          if (ch1 == END_OF_READER || ch2 == END_OF_READER)
          {
            break;
          }
          else if (ch1 == '\0' && ch2 == '\0')
          {
            break;
          }
          str += static_cast<char>(ch1);
          str += static_cast<char>(ch2);
        }
      } while (true);
      return str;
    }

    id3::string readText(size_t numChars)
    {
      id3::string str;
      if (this->getEncoding() == ID3TE_ASCII)
      {
        str.reserve(numChars);
        for (size_t i = 0; i < numChars; ++i)
        {
          int_type ch = this->readChar();
          if (ch == END_OF_READER)
          {
            break;
          }
          str += static_cast<char>(ch);
        }
      }
      else if (this->getEncoding() == ID3TE_UNICODE)
      {
        str.reserve(numChars * 2);
        for (size_t i = 0; i < numChars; ++i)
        {
          int_type ch1 = this->readChar();
          int_type ch2 = this->readChar();
          if (ch1 == END_OF_READER || ch2 == END_OF_READER)
          {
            break;
          }
          str += static_cast<char>(ch1);
          str += static_cast<char>(ch2);
        }
      }
      return str;
    }
  };

  class BinaryReader : public IdentityReader
  {
    typedef IdentityReader SUPER;
   public:
    BinaryReader(ID3_Reader& reader) : SUPER(reader) { ; }
    virtual ~BinaryReader() { ; }

    id3::bstring getBinary()
    {
      // parse the remaining bytes in this reader.  give it the largest
      // possible value for number of bytes, even though the actual number
      // of bytes might be less
      return this->getBinary(this->getEnd() - this->getCur());
    }

    id3::bstring getBinary(size_t numChars)
    {
      uchar raw[numChars];
      size_t actualSize = this->readChars(raw, numChars);
      id3::bstring binary(raw, actualSize);
      return binary;
    }
  };
};

#endif /* _ID3LIB_READER_DECORATORS_H_ */
