// -*- C++ -*-
// $Id: reader_decorators.h,v 1.5 2000/10/09 01:55:51 eldamitri Exp $

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

namespace dami
{
  namespace io
  {
    class IdentityReader : public ID3_Reader
    {
      ID3_Reader& _reader;
     public:
      IdentityReader(ID3_Reader& reader) : _reader(reader) { }
      virtual ~IdentityReader() { ; }
      
      virtual void close() { _reader.close(); }
      virtual pos_type getBeg() { return _reader.getBeg(); }
      virtual pos_type getEnd() { return _reader.getEnd(); }
      virtual pos_type getCur() { return _reader.getCur(); }
      virtual pos_type setCur(pos_type pos) { return _reader.setCur(pos); }
      
      virtual int_type readChar() { return _reader.readChar(); }
      virtual int_type peekChar() { return _reader.peekChar(); }
      virtual size_type readChars(char_type buf[], size_type len)
      { return _reader.readChars(buf, len); }
      virtual size_type readChars(char buf[], size_type len)
      { return this->readChars((uchar*) buf, len); }

      virtual size_type skipChars(size_type len) 
      { return _reader.skipChars(len); }

      virtual size_type remainingChars() { return _reader.remainingChars(); }
      //virtual bool atEnd() { return _reader.atEnd(); }
    };

    /**
     **/
    class ExitTrigger
    {
      ID3_Reader::pos_type _pos;
      ID3_Reader& _reader;
      typedef IdentityReader SUPER;
     public:
      ExitTrigger(ID3_Reader& rdr) : _reader(rdr), _pos(rdr.getCur()) { ; }
      ExitTrigger(ID3_Reader& rdr, ID3_Reader::pos_type pos) 
        : _reader(rdr), _pos(pos) {;}
      virtual ~ExitTrigger() { _reader.setCur(_pos); }
    
      void setExitPos(ID3_Reader::pos_type pos) { _pos = pos; }
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
  
      WindowedReader(ID3_Reader& reader, size_type size) 
      : SUPER(reader),
      _beg(reader.getBeg()),
      _end(reader.getEnd())
      {
        this->setWindow(this->getCur(), size);
      }
  
      WindowedReader(ID3_Reader& reader, pos_type beg, size_type size) 
      : SUPER(reader),
      _beg(reader.getBeg()),
      _end(reader.getEnd())
      {
        this->setWindow(beg, size);
      }

      void closeWindow()
      {
        this->setCur(this->getEnd());
      }
  
      void setWindow(pos_type beg, size_type size)
      {
        ID3D_NOTICE( "WindowedReader::setWindow() [beg, size] = [" << 
                     this->getBeg() << ", " << size << "]" );
        pos_type cur = this->getCur();

        // reset the end marker so as to avoid errors
        this->setEnd(SUPER::getEnd());

        // set the beginning marker
        this->setBeg(beg);

        // since the characters might be more than a byte in size, we need to 
        // manually get all the chars to set the window appropriately
        this->setCur(beg);
        ID3D_NOTICE( "WindowedReader::setWindow(): after setCur(beg), cur = "<<
                     this->getCur() );
      
        this->skipChars(size);
        ID3D_NOTICE( "WindowedReader::setWindow(): after skipChars, cur = " <<
                     this->getCur() );

        this->setEnd(this->getCur());

        ID3D_NOTICE( "WindowedReader::setWindow() [beg, cur, end] = [" << this->getBeg() << ", " << this->getCur() << ", " << this->getEnd() << "]" );


        // reset the stream
        this->setCur(cur);
      }

      pos_type setBeg(pos_type beg)
      {
        // make sure the position we want to set to isn't past the current
        // end position or the superclass's beginning position
        if (beg <= this->getEnd() && beg >= SUPER::getBeg())
        {
          _beg = beg;
        }
        else if (beg > this->getEnd())
        {
          ID3D_WARNING( "WindowedReader::setBeg() failed, [beg, _end] = " << 
                        beg << ", " << this->getEnd() << "]" );
        }
        else
        {
          ID3D_WARNING( "WindowedReader::setBeg() failed, [beg, _beg] = " << 
                        beg << ", " << this->getBeg() << "]" );
        }
        return _beg;
      }
      pos_type setEnd(pos_type end)
      {
        // make sure the position we want to set to isn't beforen the current
        // beginning position or the superclass's end position
        if (this->getBeg() <= end && end <= SUPER::getEnd())
        {
          _end = end;
        }
        else
        {
          ID3D_WARNING( "WindowedReader::setEnd() failed, end = " << end );
          ID3D_WARNING( "WindowedReader::setEnd() failed, beg = " << 
                        this->getBeg() );
          ID3D_WARNING( "WindowedReader::setEnd() failed, super.end = " << 
                        SUPER::getEnd() );
        
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
          ch = SUPER::readChar();
        }
        else
        {
          ID3D_WARNING( "io::WindowedReader::readChar: not in window, " << 
                        "pos = " << this->getCur() << ", window = [" << 
                        this->getBeg() << ", " << this->getEnd() << "]");
        }
        return ch;
      }

      int_type peekChar()
      {
        int_type ch = END_OF_READER;
        if (this->inWindow())
        {
          ch = SUPER::peekChar();
        }
        return ch;
      }

      size_type readChars(char_type buf[], size_type len)
      {
        pos_type cur = this->getCur();
        size_type size = 0;
        if (this->inWindow(cur))
        {
          size = SUPER::readChars(buf, min<size_type>(len, _end - cur));
        }
        return size;
      }

      size_type remainingChars()
      {
        pos_type cur = this->getCur();
        ID3D_NOTICE( "WindowedReader::remainingChars(): cur = " << cur );
        size_type size = this->skipChars(this->getEnd() - cur);
        ID3D_NOTICE( "WindowedReader::remainingChars(): size = " << size );
        this->setCur(cur);
        return size;
      };

      bool atEnd()
      {
        return this->getCur() >= _end;
      }
    };

    class CharReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
     public:

      CharReader(ID3_Reader& reader) : SUPER(reader) { }
      virtual ~CharReader() { ; }
    
      size_type skipChars(size_type len)
      {
        ID3D_NOTICE( "CharReader::skipChars(): len = " << len );
        return this->readChars(NULL, len);
      }

      /**
       * Read \c len characters into the array \c buf.  Since the stream needs
       * might have been unsynced, this function copies the characters one at a
       * time.
       */
      size_type readChars(char_type buf[], size_type len)
      {
        size_type numChars = 0;
        ID3D_NOTICE( "CharReader::readChars(): len = " << len );
        for (; numChars < len; ++numChars)
        {
          if (this->atEnd())
          {
            break;
          }
          char_type ch = this->readChar();
          if (buf != NULL)
          {
            buf[numChars] = ch;
          }
        }
        ID3D_NOTICE( "CharReader::readChars(): numChars = " << len );
        return numChars;
      }

      size_type remainingChars()
      {
        pos_type cur = this->getCur();
        ID3D_NOTICE( "CharReader::remainingChars(): cur = " << cur );
        size_type size = this->skipChars(this->getEnd() - cur);
        ID3D_NOTICE( "CharReader::remainingChars(): size = " << size );
        this->setCur(cur);
        return size;
      }
    };


    class LineFeedReader : public CharReader
    {
      typedef CharReader SUPER;
     public:
      LineFeedReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~LineFeedReader() { ; }

      int_type readChar()
      {
        if (this->atEnd())
        {
          return END_OF_READER;
        }
        char_type ch = SUPER::readChar();
        if (ch == 0x0D && this->peekChar() == 0x0A)
        {
          ID3D_NOTICE( "LineFeedReader::readChar(): found CRLF at pos " << 
                       this->getCur() );
          ch = SUPER::readChar();
        }
        return ch;
      };
    };

    class UnsyncedReader : public CharReader
    {
      typedef CharReader SUPER;
     public:

      UnsyncedReader(ID3_Reader& reader) : SUPER(reader) { }
      int_type readChar()
      {
        if (this->atEnd())
        {
          return END_OF_READER;
        }
        char_type ch = SUPER::readChar();
        if (ch == 0xFF && this->peekChar() == 0x00)
        {
          ID3D_NOTICE( "UnsyncedReader::readChar(): found sync at pos " << 
                       this->getCur() );
          SUPER::readChar();
        }
        return ch;
      }
    };

    class TrailingSpacesReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
     public:

      TrailingSpacesReader(ID3_Reader& reader) : SUPER(reader) { }
      String readString(size_type len);
    };

    class BinaryNumberReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
     public:
      BinaryNumberReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~BinaryNumberReader() { ; }

      uint32 readNumber(size_type numChars)
      {
        uint32 val = 0;
        char_type bytes[numChars];
        this->readChars(bytes, numChars);
        
        for (size_type i = 0; i < numChars; i++)
        {
          val *= 256; // 2^8
          val += static_cast<uint32>(bytes[i]);
        }
        return val;
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

      String readText()
      {
        String str;
        do
        {
          if (this->getEncoding() == ID3TE_ASCII)
          {
            if (this->atEnd())
            {
              break;
            }
            char_type ch = this->readChar();
            if (ch == '\0')
            {
              break;
            }
            str += static_cast<char>(ch);
          }
          else if (this->getEncoding() == ID3TE_UNICODE)
          {
            if (this->atEnd())
            {
              break;
            }
            char_type ch1 = this->readChar();
            if (this->atEnd())
            {
              break;
            }
            char_type ch2 = this->readChar();
            if (ch1 == '\0' && ch2 == '\0')
            {
              break;
            }
            str += static_cast<char>(ch1);
            str += static_cast<char>(ch2);
          }
        } while (true);
        return str;
      }

      String readText(size_type numChars)
      {
        String str;
        if (this->getEncoding() == ID3TE_ASCII)
        {
          str.reserve(numChars);
          for (size_type i = 0; i < numChars; ++i)
          {
            if (this->atEnd())
            {
              break;
            }
            char_type ch = this->readChar();
            str += static_cast<char>(ch);
          }
        }
        else if (this->getEncoding() == ID3TE_UNICODE)
        {
          str.reserve(numChars * 2);
          for (size_type i = 0; i < numChars; ++i)
          {
            if (this->atEnd())
            {
              break;
            }
            char_type ch1 = this->readChar();
            if (this->atEnd())
            {
              break;
            }
            char_type ch2 = this->readChar();
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

      BString readBinary()
      {
        // parse the remaining bytes in this reader.  give it the largest
        // possible value for number of bytes, even though the actual number
        // of bytes might be less
        return this->readBinary(this->getEnd() - this->getCur());
      }

      BString readBinary(size_type numChars)
      {
        char_type raw[numChars];
        size_type actualSize = this->readChars(raw, numChars);
        BString binary(reinterpret_cast<char*>(raw), actualSize);
        return binary;
      }
    };
    class StringReader : public ID3_Reader
    {
      pos_type _cur;
      const String&  _string;
     public:
      StringReader(const String& string) : _string(string), _cur(0) { ; }
      virtual ~StringReader() { ; }

      virtual void close() { ; }
      virtual int_type peekChar() 
      { 
        if (!this->atEnd())
        {
          return _string[_cur];
        }
        return END_OF_READER;
      }
    
      /** Read up to \c len chars into buf and advance the internal position
       ** accordingly.  Returns the number of characters read into buf.
       **/
      virtual size_type readChars(char_type buf[], size_type len)
      {
        size_type size = min(len, _string.size() - _cur);
        _string.copy((char *)buf, size, _cur);
        _cur += size;
        return size;
      }
      
      virtual pos_type getCur() 
      { 
        return _cur;
      }
      
      virtual pos_type getBeg()
      {
        return 0;
      }
      
      virtual pos_type getEnd()
      {
        return _string.size();
      }
      
      /** Set the value of the internal position for reading.
       **/
      virtual pos_type setCur(pos_type pos)
      {
        pos_type end = this->getEnd();
        _cur = (pos < end) ? pos : end;
        return _cur;
      }

      virtual bool atEnd()
      {
        return _cur >= _string.size();
      }

      virtual size_type skipChars(size_type len)
      {
        size_type size = min(len, _string.size() - _cur);
        _cur += size;
        return size;
      }
    };
  };
};

#endif /* _ID3LIB_READER_DECORATORS_H_ */
